import organoid_identifier as oi

sample_file = "example_data/Overlay_BK52_WT_BGR.tif"

print("--- MODÜL 4 TEST (STRIP OFFSET & BYTE COUNTS) ---")
oi.strip_info(sample_file)