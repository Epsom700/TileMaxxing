import torch
import torch.nn as nn
import torch.nn.functional as F


class ResidualNode(nn.Module):
    def __init__(self, channels):
        super(ResidualNode, self).__init__()
        self.conv1 = nn.Conv2d(channels, channels, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(channels, channels, kernel_size=3, padding=1)
    
    def forward(self, x):
        residual = x
        out = F.relu(self.conv1(x))
        out = self.conv2(out)
        return F.relu(out + residual)


class TileValueNet(nn.Module):
    def __init__(self):
        super(TileValueNet, self).__init__()
        self.conv_in = nn.Conv2d(17, 64, kernel_size=3, padding=1)
        self.block1 = ResidualNode(64)
        self.block2 = ResidualNode(64)
        self.conv_reduce = nn.Conv2d(64, 32, kernel_size=1)
        self.fc1 = nn.Linear(32 * 4 * 4, 128)
        self.fc2 = nn.Linear(128, 1)

    def forward(self, x): 
        x  = F.relu(self.conv_in(x))
        x = self.block1(x)
        x = self.block2(x)
        x = F.relu(self.conv_reduce(x))
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        return x.squeeze(-1)
    


if __name__ == "__main__":
    model = TileValueNet()
    dummy_input = torch.randn(2, 17, 4, 4)  # batch of 2 samples
    output = model(dummy_input)
    print("Output shape:", output.shape)
    print("Params:", sum(p.numel() for p in model.parameters()))  # should be (2,) for batch of 2 samples

    
    

