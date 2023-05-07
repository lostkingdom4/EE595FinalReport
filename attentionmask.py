import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
from torch.nn.utils.rnn import pad_sequence
from transformers import BertTokenizer, BertForMaskedLM, BertConfig
import pandas as pd
import networkx as nx
class CustomDataset(Dataset):
    def __init__(self, inputs, attention_masks, labels):
        self.inputs = inputs
        self.attention_masks = attention_masks
        self.labels = labels

    def __len__(self):
        return len(self.labels)

    def __getitem__(self, index):
        input_ids = torch.tensor(self.inputs[index])
        attention_mask = torch.tensor(self.attention_masks[index])
        label = torch.tensor(self.labels[index])

        return input_ids, attention_mask, label
class CustomSelfAttention(nn.Module):
    def __init__(self, input_size, num_heads):
        super().__init__()
        self.input_size = input_size
        self.num_heads = num_heads
        self.multihead_attn = nn.MultiheadAttention(input_size, num_heads)

    def forward(self, input_tensor, attention_mask):
        # Compute the self-attention output with the per-token attention mask
        batch_size, sequence_length, input_size = input_tensor.size()
        attention_mask = attention_mask.unsqueeze(1).repeat(1, sequence_length, 1)
        self_attention_output, _ = self.multihead_attn(input_tensor, input_tensor, input_tensor, attn_mask=attention_mask)
        # Add the input tensor to the self-attention output
        self_attention_output += input_tensor
        return self_attention_output
def parent_matrix(G):
    print("generate_mask")
    n = G.number_of_nodes()
    edges = G.edges()
    # Initialize the matrix with all zeros
    matrix = [[0] * n for _ in range(n)]
    
    # Populate the matrix with the given edges
    for i, j in edges:
        matrix[i][j] = 1
    
    # Propagate the parent-child relationships using transitive closure
    for k in range(n):
        print(k)
        for i in range(n):
            for j in range(n):
                matrix[i][j] |= matrix[i][k] & matrix[k][j]
    
    return matrix
def relabel(G):
    print("relabeling")
    # Create a dictionary mapping old node numbers to new node numbers
    node_map = {}
    new_node_number = 0
    for old_node_number in G.nodes():
        node_map[old_node_number] = new_node_number
        new_node_number += 1

    # Relabel the nodes using the node map
    G = nx.relabel_nodes(G, node_map)
    return G
model_name = 'bert-base-uncased'
tokenizer = BertTokenizer.from_pretrained(model_name)
config = BertConfig.from_pretrained(model_name)
model = BertForMaskedLM.from_pretrained(model_name, config=config)
model.bert.encoder.layer[0].attention.self = CustomSelfAttention(config.hidden_size, config.num_attention_heads)
graphs = nx.read_gpickle("graphs.gpickle")
print(len(graphs))
num = 0
inputs = []
attention_masks = []
for name, graph in graphs.items():
    if graph.number_of_nodes() < 2000:
        num += 1
    print("Graph:", graph.number_of_nodes())
    graph = relabel(graph)
    attention_mask = parent_matrix(graph)
    print(attention_mask)
    node_feature = 'operation'
    for node in graph.nodes():
        if node_feature not in graph.nodes[node]:
            graph.nodes[node][node_feature] = ''
    concatenated_string = ' '.join(nx.get_node_attributes(graph, 'operation').values())
    print(concatenated_string)
    inputs.append(concatenated_string)
    attention_masks.append(attention_mask)
    
    
print(list(graphs.keys())[0])
print(graphs[list(graphs.keys())[0]].nodes.data())
'''
# Fine-tune the model
train_dataset = 
optimizer = torch.optim.Adam(model.parameters(), lr=1e-5)
for epoch in range(num_epochs):
    for batch in train_dataset:
        input_ids, attention_mask, labels = batch
        optimizer.zero_grad()
        outputs = model(input_ids=input_ids, attention_mask=attention_mask, labels=labels)
        loss = outputs.loss
        loss.backward()
        optimizer.step()
'''