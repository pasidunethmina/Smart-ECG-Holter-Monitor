# code/inference.py
import json

def input_handler(data, context):
    """
    data: byte stream of request body
    context: provides request_content_type, etc.
    Returns: JSON string for TF Serving ({"instances": ...})
    """
    body = data.read()
    ctype = getattr(context, "request_content_type", "application/json")

    if ctype == "application/json":
        parsed = json.loads(body.decode("utf-8"))
        if isinstance(parsed, dict) and "instances" in parsed:
            payload = parsed
        else:
            payload = {"instances": parsed}
        return json.dumps(payload)

    elif ctype == "text/csv":
        rows = body.decode("utf-8").strip().splitlines()
        instances = [[float(x) for x in r.split(",")] for r in rows if r]
        return json.dumps({"instances": instances})

    else:
        raise ValueError(f"Unsupported content type: {ctype}")

def output_handler(response, context):
    """
    response: HTTP response object from TF Serving
    Returns: (body, content_type)
    """
    if response.status_code != 200:
        raise RuntimeError(
            f"TF Serving error {response.status_code}: {response.content.decode('utf-8')}"
        )
    return response.content, "application/json"
