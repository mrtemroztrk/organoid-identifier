import organoid_identifier as oi

print("=== TEST: ORGANOID C METRICS ===")
width, height = 10, 10
# 10x10 binary mask with a 4x4 square organoid in center (x: 3..6, y: 3..6)
mask = bytearray(width * height)
for y in range(3, 7):
    for x in range(3, 7):
        mask[y * width + x] = 255

metrics = oi.calculate_metrics(bytes(mask), width, height)
print("Calculated C Metrics:", metrics)
assert metrics["area"] == 16, f"Expected 16 area, got {metrics['area']}"
assert metrics["perimeter"] == 12, f"Expected 12 perimeter, got {metrics['perimeter']}"
print("[✓] Organoid C Metrics test passed successfully!")
