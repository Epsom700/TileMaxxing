import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from dataset import TileDataset, load_tile_data, group_by_game
from model import TileValueNet
import os
import random
import json
import numpy as np


class TrainingLoop:
    def __init__(self, lr=1e-3, batch_size=256, num_epochs=100, val_games=20):
        """Initialize training loop with hyperparameters."""
        self.lr = lr
        self.batch_size = batch_size
        self.num_epochs = num_epochs
        self.val_games = val_games
        
        # Hardware detection
        self.device = torch.device("mps" if torch.backends.mps.is_available() else "cpu")
        print(f"Using device: {self.device}")
        
        # Reproducibility
        torch.manual_seed(42)
        random.seed(42)
        np.random.seed(42)
        
        # Initialize components
        self.model = None
        self.optimizer = None
        self.loss_fn = None
        self.train_loader = None
        self.val_loader = None
        self.best_val_loss = float('inf')
    
    def load_data(self, jsonl_path=".data/run_1779711484.jsonl"):
        """Load and prepare data."""
        # Load all records from JSONL
        records = load_tile_data(jsonl_path)
        
        # Group by game id
        games_dict = group_by_game(records)
        game_ids = list(games_dict.keys())
        
        # Shuffle the game ids
        random.shuffle(game_ids)
        
        # Split: last val_games are val, rest are train
        train_game_ids = game_ids[:-self.val_games]
        val_game_ids = game_ids[-self.val_games:]
        
        # Build train_records and val_records by filtering
        train_records = [r for r in records if r.get('game_id') in set(train_game_ids)]
        val_records = [r for r in records if r.get('game_id') in set(val_game_ids)]
        
        # Build datasets
        train_dataset = TileDataset(train_records, augment=True)
        val_dataset = TileDataset(val_records, augment=False)
        
        # Build data loaders
        self.train_loader = DataLoader(
            train_dataset,
            batch_size=self.batch_size,
            shuffle=True,
            num_workers=0
        )
        self.val_loader = DataLoader(
            val_dataset,
            batch_size=self.batch_size,
            shuffle=False,
            num_workers=0
        )
        
        print(f"Train games: {len(train_game_ids)}, Train records: {len(train_records)}")
        print(f"Val games: {len(val_game_ids)}, Val records: {len(val_records)}")
    
    def setup_model(self):
        """Initialize model, optimizer, and loss function."""
        self.model = TileValueNet().to(self.device)
        self.optimizer = torch.optim.Adam(self.model.parameters(), lr=self.lr)
        self.loss_fn = torch.nn.HuberLoss(delta=1.0)
    
    def train(self):
        """Run the training loop."""
        os.makedirs("./model/checkpoints", exist_ok=True)
        
        for epoch in range(self.num_epochs):
            # Train phase
            self.model.train()
            train_loss_total = 0.0
            train_count = 0
            
            for x, y in self.train_loader:
                # Move to device
                x, y = x.to(self.device), y.to(self.device)
                
                # Forward pass
                self.optimizer.zero_grad()
                pred = self.model(x)
                loss = self.loss_fn(pred, y)
                
                # Backward pass
                loss.backward()
                self.optimizer.step()
                
                # Accumulate loss
                train_loss_total += loss.item() * x.size(0)
                train_count += x.size(0)
            
            train_loss = train_loss_total / train_count
            
            # Validation phase
            self.model.eval()
            val_loss_total = 0.0
            val_mae_total = 0.0
            val_count = 0
            
            with torch.no_grad():
                for x, y in self.val_loader:
                    # Move to device
                    x, y = x.to(self.device), y.to(self.device)
                    
                    # Forward pass
                    pred = self.model(x)
                    loss = self.loss_fn(pred, y)
                    
                    # Accumulate loss
                    val_loss_total += loss.item() * x.size(0)
                    val_mae_total += torch.mean(torch.abs(pred - y)).item() * x.size(0)
                    val_count += x.size(0)
            
            val_loss = val_loss_total / val_count
            val_mae = val_mae_total / val_count
            
            # Print metrics
            print(f"Epoch {epoch+1}/{self.num_epochs} | Train Loss: {train_loss:.4f} | Val Loss: {val_loss:.4f} | Val MAE: {val_mae:.4f}")
            
            # Save best model
            if val_loss < self.best_val_loss:
                self.best_val_loss = val_loss
                torch.save(self.model.state_dict(), "./model/checkpoints/best.pt")
                print(f"  → Saved best model (val_loss: {val_loss:.4f})")
        
        print("\nTraining complete!")


if __name__ == "__main__":
    # Test script
    trainer = TrainingLoop(
        lr=1e-3,
        batch_size=256,
        num_epochs=30,
        val_games=20
    )
    
    print("Loading data...")
    trainer.load_data("./data/run_1779711484.jsonl")
    
    print("Setting up model...")
    trainer.setup_model()
    
    print("Starting training...")
    trainer.train()
