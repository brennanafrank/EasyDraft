# Kye Jocham

import sys
from googleapiclient.discovery import build
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from googleapiclient.http import MediaIoBaseDownload, MediaFileUpload
import mimetypes

import os
import json

# Define the scopes
SCOPES = ['https://www.googleapis.com/auth/drive']

def authenticate():
    creds = None
    # The file token.json stores the user's access and refresh tokens.
    if os.path.exists('token.json'):
        with open('token.json', 'r') as token:
            creds_dict = json.load(token)
            creds = Credentials.from_authorized_user_info(creds_dict, SCOPES)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'credentials.json', SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open('token.json', 'w') as token:
            creds_dict = {
                'token': creds.token,
                'refresh_token': creds.refresh_token,
                'token_uri': creds.token_uri,
                'client_id': creds.client_id,
                'client_secret': creds.client_secret,
                'scopes': creds.scopes
            }
            json.dump(creds_dict, token)
    return creds

def create_easydraft():
    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)
    directory_name = "EasyDraft"

    # Check if the directory already exists
    query = f"mimeType='application/vnd.google-apps.folder' and name='{directory_name}' and trashed=false"
    results = service.files().list(q=query, fields="files(id, name)").execute()
    items = results.get('files', [])

    if not items:
        # Directory does not exist, so create it
        file_metadata = {
            'name': directory_name,
            'mimeType': 'application/vnd.google-apps.folder'
        }
        folder = service.files().create(body=file_metadata, fields='id').execute()
        print(f"Directory '{directory_name}' created with ID: {folder.get('id')}")
        parent_id = folder.get('id')
    else:
        # Directory already exists
        print(f"Directory '{directory_name}' already exists with ID: {items[0].get('id')}")
        parent_id = items[0].get('id')

    # Create the subfolders
    subfolders = ['templates', 'documents']
    for subfolder_name in subfolders:
        subfolder_query = f"mimeType='application/vnd.google-apps.folder' and name='{subfolder_name}' and parents='{parent_id}' and trashed=false"
        subfolder_results = service.files().list(q=subfolder_query, fields="files(id, name)").execute()
        subfolder_items = subfolder_results.get('files', [])

        if not subfolder_items:
            # Subfolder does not exist, so create it
            subfolder_metadata = {
                'name': subfolder_name,
                'mimeType': 'application/vnd.google-apps.folder',
                'parents': [parent_id]
            }
            subfolder = service.files().create(body=subfolder_metadata, fields='id').execute()
            print(f"Subfolder '{subfolder_name}' created with ID: {subfolder.get('id')}")
        else:
            # Subfolder already exists
            print(f"Subfolder '{subfolder_name}' already exists with ID: {subfolder_items[0].get('id')}")


def select_template(file_path):
    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)

    # Extract the file name from the file path
    file_name = os.path.basename(file_path)

    # Check if the file exists in the templates folder
    templates_query = "mimeType='application/vnd.google-apps.folder' and name='templates' and trashed=false"
    templates_results = service.files().list(q=templates_query, fields="files(id, name)").execute()
    templates_items = templates_results.get('files', [])

    if not templates_items:
        print("Templates folder not found in EasyDraft.")
        return

    templates_folder_id = templates_items[0].get('id')

    # Check if the file already exists in the templates folder
    templates_file_query = f"name='{file_name}' and parents='{templates_folder_id}' and trashed=false"
    templates_file_results = service.files().list(q=templates_file_query, fields="files(id, name)").execute()
    templates_file_items = templates_file_results.get('files', [])

    if templates_file_items:
        print(f"File '{file_name}' already exists in the templates folder.")
        return

    # Check if the file exists elsewhere in Google Drive
    query = f"name='{file_name}' and trashed=false"
    results = service.files().list(q=query, fields="files(id, name, parents)").execute()
    items = results.get('files', [])

    if not items:
        print(f"File '{file_name}' not found in Google Drive.")
        return

    # Find the file that matches the specified file path
    for item in items:
        parent_id = item.get('parents')[0] if item.get('parents') else None
        if parent_id:
            parent_query = f"'{parent_id}' in parents and trashed=false"
            parent_results = service.files().list(q=parent_query, fields="files(id, name)").execute()
            parent_items = parent_results.get('files', [])
            parent_name = parent_items[0].get('name') if parent_items else None
            full_path = os.path.join(parent_name, file_name) if parent_name else file_name
            if full_path == file_path:
                # Copy the file to the templates folder
                file_id = item.get('id')
                file_metadata = {
                    'name': file_name,
                    'parents': [templates_folder_id]
                }
                copied_file = service.files().copy(fileId=file_id, body=file_metadata).execute()
                print(f"File '{file_name}' copied to templates folder with ID: {copied_file.get('id')}")
                return
        elif file_name == file_path:
            # The file is in the root of Google Drive
            file_id = item.get('id')
            file_metadata = {
                'name': file_name,
                'parents': [templates_folder_id]
            }
            copied_file = service.files().copy(fileId=file_id, body=file_metadata).execute()
            print(f"File '{file_name}' copied to templates folder with ID: {copied_file.get('id')}")
            return

    print(f"File '{file_name}' not found at specified path: {file_path}")


def download_template(file_name, local_directory):
    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)

    # Check if the file exists in the templates folder
    templates_query = "mimeType='application/vnd.google-apps.folder' and name='templates' and trashed=false"
    templates_results = service.files().list(q=templates_query, fields="files(id, name)").execute()
    templates_items = templates_results.get('files', [])

    if not templates_items:
        print("Templates folder not found in EasyDraft.")
        return

    templates_folder_id = templates_items[0].get('id')

    # Check if the file exists in the templates folder
    file_query = f"name='{file_name}' and parents='{templates_folder_id}' and trashed=false"
    file_results = service.files().list(q=file_query, fields="files(id, name, mimeType)").execute()
    file_items = file_results.get('files', [])

    if not file_items:
        print(f"File '{file_name}' not found in the templates folder.")
        return

    file_id = file_items[0].get('id')
    mime_type = file_items[0].get('mimeType')

    # Create the full local file path
    if mime_type == 'application/vnd.google-apps.document':
        local_file_path = os.path.join(local_directory, file_name + '.docx')
    else:
        local_file_path = os.path.join(local_directory, file_name)

    # Download or export the file
    if mime_type == 'application/vnd.google-apps.document':
        request = service.files().export_media(fileId=file_id, mimeType='application/vnd.openxmlformats-officedocument.wordprocessingml.document')
    else:
        request = service.files().get_media(fileId=file_id)

    with open(local_file_path, 'wb') as f:
        downloader = MediaIoBaseDownload(f, request)
        done = False
        while done is False:
            status, done = downloader.next_chunk()
            print(f"Download {int(status.progress() * 100)}%.")

    print(f"File '{file_name}' downloaded to {local_file_path}")


def upload_document(file_name, local_directory):
    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)

    # Create the full local file path
    local_file_path = os.path.join(local_directory, file_name)

    # Check if the file exists locally
    if not os.path.exists(local_file_path):
        print(f"File '{file_name}' not found in the local directory '{local_directory}'.")
        return

    # Get the ID of the documents folder
    documents_query = "mimeType='application/vnd.google-apps.folder' and name='documents' and trashed=false"
    documents_results = service.files().list(q=documents_query, fields="files(id, name)").execute()
    documents_items = documents_results.get('files', [])

    if not documents_items:
        print("Documents folder not found in EasyDraft.")
        return

    documents_folder_id = documents_items[0].get('id')

    # Convert the file to PDF if necessary
    file_mime_type, _ = mimetypes.guess_type(local_file_path)
    if file_mime_type != 'application/pdf':
        print(f"Converting '{file_name}' to PDF is not supported. Only PDF files can be uploaded.")
        return

    # Upload the file to the documents folder
    file_metadata = {
        'name': file_name,
        'parents': [documents_folder_id]
    }
    media = MediaFileUpload(local_file_path, mimetype='application/pdf')
    uploaded_file = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    print(f"File '{file_name}' uploaded as PDF to documents folder with ID: {uploaded_file.get('id')}")


def delete_file(file_name, folder_name):
    if folder_name not in ["templates", "documents"]:
        print(f"Invalid folder name: {folder_name}. Must be 'templates' or 'documents'.")
        return

    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)

    # Check if the EasyDraft folder exists
    query = "mimeType='application/vnd.google-apps.folder' and name='EasyDraft' and trashed=false"
    results = service.files().list(q=query, fields="files(id, name)").execute()
    easydraft_items = results.get('files', [])

    if not easydraft_items:
        print("EasyDraft folder not found.")
        return

    easydraft_folder_id = easydraft_items[0].get('id')

    # Check if the specified folder exists within EasyDraft
    folder_query = f"mimeType='application/vnd.google-apps.folder' and name='{folder_name}' and parents='{easydraft_folder_id}' and trashed=false"
    folder_results = service.files().list(q=folder_query, fields="files(id, name)").execute()
    folder_items = folder_results.get('files', [])

    if not folder_items:
        print(f"'{folder_name}' folder not found in EasyDraft.")
        return

    folder_id = folder_items[0].get('id')

    # Check if the file exists in the specified folder
    file_query = f"name='{file_name}' and parents='{folder_id}' and trashed=false"
    file_results = service.files().list(q=file_query, fields="files(id, name)").execute()
    file_items = file_results.get('files', [])

    if not file_items:
        print(f"File '{file_name}' not found in the '{folder_name}' folder.")
        return

    file_id = file_items[0].get('id')

    # Delete the file
    service.files().delete(fileId=file_id).execute()
    print(f"File '{file_name}' in the '{folder_name}' folder has been deleted.")


def delete_easydraft():
    creds = authenticate()
    service = build('drive', 'v3', credentials=creds)

    # Check if the EasyDraft folder exists
    query = "mimeType='application/vnd.google-apps.folder' and name='EasyDraft' and trashed=false"
    results = service.files().list(q=query, fields="files(id, name)").execute()
    items = results.get('files', [])

    if not items:
        print("EasyDraft folder not found.")
        return

    easydraft_folder_id = items[0].get('id')

    # Delete the EasyDraft folder
    service.files().delete(fileId=easydraft_folder_id).execute()
    print("EasyDraft folder and all its contents have been deleted.")


if __name__ == '__main__':
    num_args = len(sys.argv)
    if num_args in [0, 1]:
        print("Usage: python google_drive.py <function> [arguments]")
    elif num_args == 2:  # No arguments for the function
        function_name = sys.argv[1]
        if function_name == 'create_easydraft':
            create_easydraft()
        elif function_name == 'delete_easydraft':
            delete_easydraft()
        else:
            print("Issue 2")
    elif num_args == 3:  # One argument for the function
        function_name = sys.argv[1]
        arg_one = sys.argv[2]
        if function_name == 'select_template':
            select_template(arg_one)
        else:
            print("Issue 3")
    elif num_args == 4:  # Two arguments for one function
        function_name = sys.argv[1]
        arg_one = sys.argv[2]
        arg_two = sys.argv[3]
        if function_name == 'download_template':
            download_template(arg_one, arg_two)
        elif function_name == 'upload_document':
            upload_document(arg_one, arg_two)
        elif function_name == 'delete_file':
            delete_file(arg_one, arg_two)
        else:
            print("Issue 4")
    else:
        print("Issue 1")
