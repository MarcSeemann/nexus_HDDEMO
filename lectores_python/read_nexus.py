

import h5py
import pandas as pd
import matplotlib.pyplot as plt

filename = "NextHDDEMO_full.next.h5"

with h5py.File(filename, "r") as f:
    # Ver estructura del fichero
    print(list(f.keys()))
    print(list(f["MC"].keys()))

    # Leer MC/hits
    hits_data = f["MC/hits"][:]
    df_hits = pd.DataFrame(hits_data)

    # Leer MC/particles
    particles_data = f["MC/particles"][:]
    df_particles = pd.DataFrame(particles_data)

# Decodificar columnas de texto (h5py las da como bytes, no como str)
for col in df_hits.select_dtypes([object]).columns:
    df_hits[col] = df_hits[col].apply(lambda x: x.decode() if isinstance(x, bytes) else x)

for col in df_particles.select_dtypes([object]).columns:
    df_particles[col] = df_particles[col].apply(lambda x: x.decode() if isinstance(x, bytes) else x)

print(df_hits.head())
print(df_particles.head())

# Ver labels disponibles
print(df_hits["label"].unique())

# Filtrar volumen activo
df_active = df_hits[df_hits["label"] == "ACTIVE"]

# Energía total depositada por evento
energy_per_event = df_active.groupby("event_id")["energy"].sum()

# Histograma
plt.hist(energy_per_event, bins=100)
plt.xlabel("Energía depositada (MeV)")
plt.ylabel("Eventos")
plt.title("Energía depositada en el volumen activo")
plt.savefig("histograma_energia.png")


print("\n===== EVENTOS =====")
print("Eventos únicos:", df_hits["event_id"].nunique())
