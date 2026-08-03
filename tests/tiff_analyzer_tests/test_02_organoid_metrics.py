import organoid_identifier as oi

print("=== TEST: ORGANOID C METRICS & SIGNAL INTENSITY ===")
width, height = 10, 10
# 10x10 binary mask with a 4x4 square organoid in center (x: 3..6, y: 3..6)
mask = bytearray(width * height)
# 10x10 image with pixel intensity = 200 inside organoid
img = bytearray(width * height)

for y in range(3, 7):
    for x in range(3, 7):
        idx = y * width + x
        mask[idx] = 255
        img[idx] = 200

metrics = oi.calculate_metrics(bytes(mask), width, height, bytes(img), 1)
print("Calculated C Metrics:", metrics)

assert metrics["area"] == 16, f"Expected 16 area, got {metrics['area']}"
assert metrics["perimeter"] == 12, f"Expected 12 perimeter, got {metrics['perimeter']}"
assert metrics["mean_intensity"] == 200.0, f"Expected 200.0 mean intensity, got {metrics['mean_intensity']}"
assert metrics["integrated_intensity"] == 3200.0, f"Expected 3200.0 integrated intensity, got {metrics['integrated_intensity']}"
assert metrics["min_intensity"] == 200.0, f"Expected 200.0 min intensity, got {metrics['min_intensity']}"
assert metrics["max_intensity"] == 200.0, f"Expected 200.0 max intensity, got {metrics['max_intensity']}"
assert metrics["std_intensity"] == 0.0, f"Expected 0.0 std intensity, got {metrics['std_intensity']}"

print("[✓] Organoid C Signal Intensity Metrics test passed successfully!")
