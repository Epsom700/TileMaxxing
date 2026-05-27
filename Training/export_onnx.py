import torch
from model import TileValueNet

CHECKPOINT = "./model/checkpoints/best.pt"
OUTPUT     = "./model/tile_value_net.onnx"

model = TileValueNet()
model.load_state_dict(torch.load(CHECKPOINT, map_location="cpu"))
model.eval()

dummy = torch.randn(1, 17, 4, 4)
print("dummy output:", model(dummy))

torch.onnx.export(
    model, dummy, OUTPUT,
    input_names=["board"],
    output_names=["value"],
    dynamic_axes={"board": {0: "batch"}, "value": {0: "batch"}},
    opset_version=17,
)
print(f"Exported to {OUTPUT}")
