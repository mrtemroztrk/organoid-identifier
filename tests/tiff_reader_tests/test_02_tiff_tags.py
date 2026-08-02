import organoid_identifier as oi

sample_file = "example_data/Overlay_BK52_9806_BGR.tif"

print("==================================================")
print(" 1. MODÜL TESTİ (TIFF HEADER INSPECTION)")
print("==================================================")
oi.inspect(sample_file)

print("\n==================================================")
print(" 2. MODÜL TESTİ (TIFF DIMENSIONS & TAGS)")
print("==================================================")
oi.dimensions(sample_file)