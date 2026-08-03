import os
import sys
import shutil
import urllib.request
import urllib.error

# Default user-visible cache directory in home directory
DEFAULT_CACHE_DIR = os.path.expanduser("~/.organoid_identifier/models")

# Official CellPose model endpoints
CELLPOSE_MODELS = {
    "cyto3": "https://www.cellpose.org/models/cyto3",
    "cyto2": "https://www.cellpose.org/models/cyto2",
    "cyto": "https://www.cellpose.org/models/cyto",
    "nuclei": "https://www.cellpose.org/models/nuclei",
    "organoid": "https://www.cellpose.org/models/cpsami_organoid",
    "cpsami_organoid": "https://www.cellpose.org/models/cpsami_organoid"
}

def get_cache_dir(custom_dir: str = None) -> str:
    """Returns and ensures the user-visible cache directory exists."""
    target_dir = os.path.abspath(custom_dir) if custom_dir else DEFAULT_CACHE_DIR
    os.makedirs(target_dir, exist_ok=True)
    return target_dir

def get_model_path(model_name: str = "cyto3", cache_dir: str = None) -> str:
    """Returns the full local file path for a given model."""
    target_dir = get_cache_dir(cache_dir)
    filename = model_name if model_name.endswith((".pth", ".pt")) else f"{model_name}.pth"
    return os.path.join(target_dir, filename)

def _format_size(size_bytes: int) -> str:
    """Formats bytes into human readable MB string."""
    return f"{size_bytes / (1024 * 1024):.2f} MB"

def download_model(model_name: str = "cyto3", force: bool = False, cache_dir: str = None) -> str:
    """
    Downloads and manages isolated CellPose models without external dependencies.
    
    Parameters:
        model_name (str): Model type identifier ('cyto3', 'cyto2', 'nuclei', 'organoid'). Default is 'cyto3'.
        force (bool): If True, redownloads and updates the model even if it exists. Default is False.
        cache_dir (str): Optional custom directory. Default is '~/.organoid_identifier/models'.
        
    Returns:
        str: Absolute file path to the downloaded model.
    """
    key = model_name.lower().strip()
    url = CELLPOSE_MODELS.get(key)
    
    if not url:
        if model_name.startswith("http://") or model_name.startswith("https://"):
            url = model_name
            filename = os.path.basename(url)
        else:
            valid_models = ", ".join(CELLPOSE_MODELS.keys())
            raise ValueError(f"Unknown model '{model_name}'. Available built-in models: {valid_models}")
    else:
        filename = f"{key}.pth"

    target_dir = get_cache_dir(cache_dir)
    dest_path = os.path.join(target_dir, filename)

    print(f"\n[+] Organoid Model Manager: '{key}'")
    print(f"[+] Local Cache Path: {dest_path}")

    # Check if model already exists
    if os.path.exists(dest_path) and not force:
        file_size = os.path.getsize(dest_path)
        print(f"[✓] Model already installed and ready ({_format_size(file_size)}).")
        print(f"[i] To re-download or update, use: download_model('{model_name}', force=True)")
        return dest_path

    if force and os.path.exists(dest_path):
        print(f"[!] Updating model: removing existing file...")
        os.remove(dest_path)

    print(f"[+] Downloading model from: {url}")
    
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'Organoid-Identifier/0.4.6'})
        with urllib.request.urlopen(req) as response, open(dest_path, 'wb') as out_file:
            total_size = response.getheader('Content-Length')
            if total_size:
                total_size = int(total_size)
            
            downloaded = 0
            block_size = 8192
            
            while True:
                buffer = response.read(block_size)
                if not buffer:
                    break
                downloaded += len(buffer)
                out_file.write(buffer)
                
                if total_size:
                    percent = (downloaded / total_size) * 100
                    sys.stdout.write(f"\r    Progress: {percent:.1f}% ({_format_size(downloaded)} / {_format_size(total_size)})")
                    sys.stdout.flush()
                else:
                    sys.stdout.write(f"\r    Downloaded: {_format_size(downloaded)}")
                    sys.stdout.flush()
                    
        print(f"\n[✓] Download completed successfully!")
        return dest_path

    except urllib.error.URLError as e:
        if os.path.exists(dest_path):
            os.remove(dest_path)
        raise RuntimeError(f"Failed to download model '{model_name}': {e}")

def list_local_models(cache_dir: str = None) -> list:
    """
    Lists all locally installed models in the user cache directory with details.
    
    Returns:
        list: List of dictionaries containing model metadata.
    """
    target_dir = get_cache_dir(cache_dir)
    print(f"\n┌───────────────────────────────────────────────────────┐")
    print(f"│            ORGANOID LOCAL MODELS REPORT               │")
    print(f"├───────────────────────────────────────────────────────┤")
    print(f"│  • Cache Directory   : {target_dir}")
    
    models = []
    if os.path.exists(target_dir):
        files = [f for f in sorted(os.listdir(target_dir)) if not f.startswith(".")]
        if not files:
            print(f"│  • Installed Models  : None (0 files)")
        else:
            print(f"├───────────────────────────────────────────────────────┤")
            for f in files:
                fpath = os.path.join(target_dir, f)
                size_str = _format_size(os.path.getsize(fpath))
                models.append({"name": f, "path": fpath, "size": size_str})
                print(f"│  • {f:<18}: {size_str:>10}  |  {fpath}")
    print(f"└───────────────────────────────────────────────────────┘\n")
    return models

def remove_model(model_name: str = "cyto3", cache_dir: str = None) -> bool:
    """
    Safely removes a specific model file from the cache directory.
    
    Parameters:
        model_name (str): Name or alias of the model to remove.
        
    Returns:
        bool: True if removed, False if file was not found.
    """
    target_dir = get_cache_dir(cache_dir)
    key = model_name.lower().strip()
    filename = key if key.endswith((".pth", ".pt")) else f"{key}.pth"
    dest_path = os.path.join(target_dir, filename)

    if os.path.exists(dest_path):
        os.remove(dest_path)
        print(f"[✓] Removed model '{filename}' from {target_dir}")
        return True
    else:
        print(f"[!] Model '{filename}' not found in {target_dir}")
        return False

def clear_models(cache_dir: str = None) -> None:
    """
    Clears all installed models from the cache directory to free up disk space.
    """
    target_dir = get_cache_dir(cache_dir)
    if os.path.exists(target_dir):
        count = 0
        for f in os.listdir(target_dir):
            fpath = os.path.join(target_dir, f)
            if os.path.isfile(fpath):
                os.remove(fpath)
                count += 1
        print(f"[✓] Cleared {count} model(s) from cache directory: {target_dir}")
    else:
        print(f"[i] Cache directory does not exist: {target_dir}")
