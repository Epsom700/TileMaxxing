import torch
from torch.utils.data import Dataset
from torch.utils.data import DataLoader
import numpy as np
import os
import json
import math




def load_tile_data(game_path): 
    game_data = []
    with open(game_path, 'r') as f: 
        for line in f: 
            try: 
                game_data.append(json.loads(line.strip()))
            except json.JSONDecodeError:
                continue
    return game_data
    
def group_by_game(game_data): 
    games = {}
    for record in game_data: 
        game_id = record['game_id']
        if game_id not in games: 
            games[game_id] = []
        games[game_id].append(record)
    return games



def dihedral_transform(tensor, k):
    # k in 0..7
    if k >= 4:
        tensor = torch.flip(tensor, dims=[-1])
    rotation = k % 4
    if rotation > 0:
        tensor = torch.rot90(tensor, rotation, dims=[-2, -1])
    return tensor


class TileDataset(Dataset): 
    def __init__(self, records, augment=False): 
        self.records = records
        self.augment = augment

        self.TILE_VALUES = [0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536]
        self.tiles_to_channel = {v: i for i, v in enumerate(self.TILE_VALUES)}

    def __len__(self): 
        return len(self.records)
    
    def __getitem__(self, idx):
        record = self.records[idx]
        board = record['board']
        board_2d = torch.tensor(board).reshape(4, 4)
        tensor = (torch.tensor(self.TILE_VALUES).view(-1, 1, 1) == board_2d).float()
        k = np.random.randint(8) if self.augment else 0
        tensor = dihedral_transform(tensor, k)
        target_tensor = torch.tensor(math.log1p(record['eval']), dtype=torch.float32)
        return tensor, target_tensor

if __name__ == "__main__":
    print(os.getcwd())
    game_data = load_tile_data('./data/run_1779711484.jsonl')
    games = group_by_game(game_data)
    print("Loaded:", len(game_data), "records across", len(games), "games.")
    # taking only the first 5 games for training, and 2 for validation
    train_game_ids = set(list(games)[:5])
    val_game_ids = set(list(games)[5:7])

    train_records = [r for r in game_data if r['game_id'] in train_game_ids]
    val_records = [r for r in game_data if r['game_id'] in val_game_ids]
    train_dataset = TileDataset(train_records, augment=True)
    val_dataset = TileDataset(val_records, augment=False)
    print("Train dataset size:", len(train_dataset))
    print("Validation dataset size:", len(val_dataset))
    x, y = train_dataset[0]
    print("Sample input shape:", x.shape, x.dtype)
    print("Sample target value:", y.item(), y.dtype)
    assert x.sum(dim=0).allclose(torch.ones(4, 4))
