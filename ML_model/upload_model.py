import boto3

# S3 client
s3 = boto3.client("s3")

# Your bucket name
bucket_name = "sagemaker-ap-south-1-076181802957"

# Upload the tar.gz file
s3.upload_file("model.tar.gz", bucket_name, "model.tar.gz")

print("✅ Model uploaded to S3 successfully!")
