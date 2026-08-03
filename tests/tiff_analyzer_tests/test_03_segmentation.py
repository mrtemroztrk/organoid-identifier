import organoid_identifier as oi

# Single line of code to perform multi-object segmentation and list all organoid objects & metrics
objects = oi.segment_info("example_data/Overlay_BK52_WT_BGR.tif", min_size=500)
print(f"[✓] Segmentation test completed! Detected {len(objects)} organoid object(s).")
