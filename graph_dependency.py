import networkx as nx
import random
import matplotlib.pyplot as plt
# Create a graph
elist = [(0,1),(1,2)]

G = nx.DiGraph()
G.add_edges_from(elist)
# Print some basic information about the graph
print("Number of nodes:", G.number_of_nodes())
print("Number of edges:", G.number_of_edges())
print("Edges with weights:")
for u, v, w in G.edges.data('weight'):
    print(u, v, w)
pos = nx.spring_layout(G)
nx.draw(G, pos, with_labels=True)
plt.show()