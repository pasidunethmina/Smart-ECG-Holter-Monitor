import tarfile

with tarfile.open("model.tar.gz", "w:gz") as tar:
    tar.add("ecg_cnn_model.keras", arcname="ecg_cnn_model.keras")