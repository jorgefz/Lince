import sys
import pandas as pd
import numpy as np
from tqdm import tqdm

assert len(sys.argv) > 1, "Error: provide profiler output file"

data = pd.read_csv(sys.argv[1], delimiter=':', names=["Function","Runtime"])
funcs = np.array(data['Function'].unique())

# Collect average runtime for each function
means = np.array([data.loc[data['Function'] == f]['Runtime'].mean() for f in tqdm(funcs)])
stds  = np.array([data.loc[data['Function'] == f]['Runtime'].std(ddof=0) for f in tqdm(funcs)])
# indices to sort runtimes from largest to shortest
idx = np.argsort(means)[::-1]

# Print out results
print("Function".ljust(30) + "Mean execution time")
for f,m,s in zip(funcs[idx], means[idx], stds[idx]):
	print(f.ljust(30) + f"{float(m):.4f} +- {float(s):.4f} ms")
