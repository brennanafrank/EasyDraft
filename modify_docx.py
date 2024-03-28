from docx import Document
import json
import sys
import os

def check_all_pairs_have_equal_size(replacements):
    """
    Check if all replacement pairs have the same number of elements.
    """
    if not replacements:
        return True

    expected_size = len(next(iter(replacements.values())))
    return all(len(v) == expected_size for v in replacements.values())

def replace_placeholders_and_save(doc_path, replacements, save_path, file_name_prefix):
    """
    Replace placeholders in the document with values from replacements
    and save new documents.
    """
    if not check_all_pairs_have_equal_size(replacements):
        print("Error: Not all placeholders have the same number of replacements.")
        return

    for idx in range(len(next(iter(replacements.values())))):
        doc = Document(doc_path)

        for paragraph in doc.paragraphs:
            for placeholder, values in replacements.items():
                if f"$${placeholder}$$" in paragraph.text:
                    paragraph.text = paragraph.text.replace(f"$${placeholder}$$", values[idx])

        new_doc_path = os.path.join(save_path, f"{file_name_prefix}_{idx+1}.docx")
        os.makedirs(os.path.dirname(new_doc_path), exist_ok=True)
        doc.save(new_doc_path)
        print(f"Document saved as: {new_doc_path}")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python script.py doc_path replacements_path save_path file_name_prefix")
        sys.exit(1)

    doc_path = sys.argv[1]
    replacements_path = sys.argv[2]
    save_path = sys.argv[3]
    file_name_prefix = sys.argv[4]

    with open(replacements_path) as f:
        replacements = json.load(f)

    replace_placeholders_and_save(doc_path, replacements, save_path, file_name_prefix)
