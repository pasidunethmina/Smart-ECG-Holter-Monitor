import tarfile
import os
import sys

def create_tar_gz():
    # List of files to include
    files_to_include = [
        "inference.py",
        "requirements.txt",
        "ecg_cnn_model.keras"  # or whatever your model file is named
    ]
    
    # Check if all files exist
    missing_files = []
    for file in files_to_include:
        if not os.path.exists(file):
            missing_files.append(file)
    
    if missing_files:
        print(f"Error: The following files are missing:")
        for file in missing_files:
            print(f"  - {file}")
        print("\nPlease make sure these files are in the same directory as this script.")
        return False
    
    # Create tar.gz file
    try:
        with tarfile.open('model.tar.gz', 'w:gz') as tar:
            # Add model file to root
            if os.path.exists('saved_model/assets'):
                tar.add('saved_model/assets', arcname='saved_model/assets')
                print("Added: saved_model/assets")
            
            # Add code files to code/ directory
            if os.path.exists('saved_model/variables/variables.data-00000-of-00001'):
                tar.add('saved_model/variables/variables.data-00000-of-00001', arcname='saved_model/variables/variables.data-00000-of-00001')
                print("Added: code/inference.py")
            
            if os.path.exists('saved_model/variables/variables.index'):
                tar.add('saved_model/variables/variables.index', arcname='saved_model/variables/variables.index')
                print("Added: saved_model/variables/variables.index")

            if os.path.exists('saved_model/fingerprint.pb'):
                tar.add('saved_model/fingerprint.pb', arcname='saved_model/fingerprint.pb')
                print("Added: saved_model/assets")

            if os.path.exists('saved_model/saved_model.pb'):
                tar.add('saved_model/saved_model.pb', arcname='saved_model/saved_model.pb')
                print("Added: saved_model/assets")
            
            if os.path.exists('inference.py'):
                tar.add('inference.py', arcname='code/inference.py')
                print("Added: code/inference.py")

            if os.path.exists('requirements.txt'):
                tar.add('requirements.txt', arcname='code/requirements.txt')
                print("Added: code/requirements.txt")

    except Exception as e:
        print(f"Error creating tar.gz file: {e}")
        return False

if __name__ == "__main__":
    create_tar_gz()