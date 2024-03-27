import json
import re
from docx import Document
import sys

def find_placeholders(doc_path):
    """
    Find all unique placeholders in the format $$key$$ in the document and return them as a JSON string.
    Each key maps to an empty list, to match the expected input format of the jsonToVector C++ function.
    """
    doc = Document(doc_path)
    placeholder_pattern = re.compile(r"\$\$(.*?)\$\$")
    placeholders = {}

    for paragraph in doc.paragraphs:
        matches = placeholder_pattern.findall(paragraph.text)
        for match in matches:
            # Ensure each key maps to an empty list
            placeholders[match] = []

    return json.dumps(placeholders)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python find_placeholders.py doc_path")
        sys.exit(1)

    doc_path = sys.argv[1]
    placeholders_json = find_placeholders(doc_path)
    print(placeholders_json)
