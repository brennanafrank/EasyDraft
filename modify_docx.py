from docx import Document
import json
import sys

def check_all_pairs_have_equal_size(replacements):
    """
    Check if all replacement pairs have the same number of elements.
    """
    if not replacements:
        return True

    expected_size = len(next(iter(replacements.values())))
    return all(len(v) == expected_size for v in replacements.values())

def replace_placeholders_and_save(doc_path, replacements):
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

        new_doc_path = f"modified_{idx+1}_{replacements['Name'][idx]}.docx"
        doc.save(new_doc_path)
        print(f"Document saved as: {new_doc_path}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py doc_path replacements_path")
        sys.exit(1)

    doc_path = sys.argv[1]
    replacements_path = sys.argv[2]

    with open(replacements_path) as f:
        replacements = json.load(f)

    replace_placeholders_and_save(doc_path, replacements)

