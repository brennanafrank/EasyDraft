from docx import Document
import sys
import os
from docx2pdf import convert



def pdfConvert(filePath_arg, dirPath_arg=None):

    #print(dirPath)

    try:
        # Convert .docx to .pdf
        convert(filePath_arg)
        #print(f"Conversion successful! Output PDF: {output_path}")
    except Exception as e:
        print(f"Conversion failed due to error: {e}")




if __name__ == '__main__':

    if len(sys.argv) != 3:
        print("Usage: python script.py doc_path replacements_path save_path file_name_prefix")
        sys.exit(1)


    dirPath = sys.argv[1]
    filePath = sys.argv[2]

    pdfConvert(filePath, "")
