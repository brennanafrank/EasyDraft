import sys
from googleapiclient.discovery import build
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
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

def create_directory():
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


if __name__ == '__main__':
    match len(sys.argv):
        case 0, 1:
            print("Usage: python google_drive.py <function> [arguments]")
        case 2:
            function_name = sys.argv[1]
            match function_name:
                case 'create_directory':
                    create_directory()
                case _:
                    print("Issue 2")
        case _:
            print("Issue 1")