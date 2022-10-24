import pandas as pd

data = pd.read_csv("profile.txt", delimiter=':', names=["Function","Runtime"])
unique_funcs = data['Function'].unique()

# Collect average runtime for each function
print("Function".ljust(30) + "Mean execution time")
for func in unique_funcs:
	mean = data.loc[data['Function'] == func]['Runtime'].mean()
	std = data.loc[data['Function'] == func]['Runtime'].std()
	print(func.ljust(30) + f"{float(mean):.4f} +- {float(std):.4f} ms")