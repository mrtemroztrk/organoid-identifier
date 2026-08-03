import organoid_identifier as oi

print("=== TEST: MODEL MANAGER LISTING ===")
models = oi.list_local_models()
print("Returned models count:", len(models))

print("\n=== TEST: GET MODEL PATH ===")
path = oi.get_model_path("cyto3")
print("Cyto3 model path:", path)
