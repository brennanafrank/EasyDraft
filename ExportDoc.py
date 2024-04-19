from docx import Document
import sys
import mammoth as mth
import os
from docx2pdf import convert
import docx2txt as d2t



def pdfConvert(filePath_arg, dirPath_arg=None):


    try:
        # Convert .docx to .pdf
        convert(filePath_arg)
    except Exception as e:
        print(f"Conversion failed due to error: {e}")


    file_name = os.path.basename(filePath_arg)

    file_name_pdf = file_name[:-5] + ".pdf"
    file_path_pdf = filePath_arg[:-5] + ".pdf"
    dirPath_pdf = dirPath + "/" + file_name_pdf

    os.rename(file_path_pdf, dirPath_pdf)


def txtConvert(filePath_arg, dirPath_arg):

    file_name = os.path.basename(filePath_arg)
    file_name_txt = file_name[:-5] + ".txt"

    docTxt = d2t.process(filePath_arg)

    fileTxt = open(dirPath_arg + "/" + file_name_txt, 'w' , encoding = 'utf-8')
    fileTxt.write(docTxt)
    fileTxt.close()


def htmlConvert(filePath_arg, dirPath_arg):

    file_name = os.path.basename(filePath_arg)
    file_name_html = file_name[:-5] + ".html"

    docHtml = mth.convert_to_html(filePath_arg)

    fileTxt = open(dirPath_arg + "/" + file_name_html, 'w' , encoding = 'utf-8')
    fileTxt.write(docHtml.value)
    fileTxt.close()


if __name__ == '__main__':

    if len(sys.argv) != 4:
        print("Usage: python script.py doc_path replacements_path save_path file_name_prefix")
        sys.exit(1)


    dirPath = sys.argv[1]
    filePath = sys.argv[2]
    typeOfExport = sys.argv[3]

    if typeOfExport == "1":

        pdfConvert(filePath, "")

    elif typeOfExport == "2":

        txtConvert(filePath, dirPath)

    elif typeOfExport == "3":

        htmlConvert(filePath, dirPath)
