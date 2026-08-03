import tables
import pandas as pd

filename = "nexus_out.h5"
file = tables.open_file(filename, mode="r")

config_node = file.get_node("/MC/configuration")
df_config = pd.DataFrame.from_records(config_node.read())
# decode byte-strings if needed
for col in df_config.columns:
    if df_config[col].dtype == object:
        df_config[col] = df_config[col].str.decode("utf-8", errors="ignore")

pd.set_option("display.max_colwidth", None)
pd.set_option("display.max_rows", None)
print(df_config)

file.close()
