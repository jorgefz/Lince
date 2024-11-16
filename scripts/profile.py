import sys
import pandas as pd
import numpy as np

assert len(sys.argv) > 1, "Error: provide profiler output file"

data = pd.read_csv(sys.argv[1], delimiter=':', names=["Function","Runtime"])

grouped = data.groupby('Function')
result = grouped.agg({'Runtime':['mean',lambda x: x.std(ddof=0)]})
result.columns  = ['mean', 'std']
result.sort_values('mean', ascending=False, inplace=True)

print("Function".ljust(30) + "Mean execution time")
for f, (m, s) in result.iterrows():
	print(f.ljust(30) + f"{float(m*1e3):.5f} +- {float(s*1e3):.5f} ms")
