import os
import json
import requests
import sys
import msal
import logging
from pathlib import Path

API_ENDPOINT = 'https://graph.microsoft.com/v1.0'

def authenticate():
    #client_id = os.getenv('ONEDRIVE_CLIENT_ID')
    client_id = "3bf17d64-7891-4b2d-879c-a9a079222497"
    #client_secret = os.getenv('ONEDRIVE_CLIENT_SECRET')
    #tenant_id = os.getenv('ONEDRIVE_TENANT_ID')
    tenant_id = "f8cdef31-a31e-4b4a-93e4-5f571e91255a"
    authority = f'https://login.microsoftonline.com/{tenant_id}'
    token_url = f'{authority}/oauth2/v2.0/token'
    scope = ['https://graph.microsoft.com/Files.ReadWrite.All']
    token_file = 'token.json'

    # Create a confidential client application
    app = msal.ConfidentialClientApplication(client_id, authority=authority, client_credential=client_secret)

    # Check if a token file exists and load it
    if os.path.exists(token_file):
        with open(token_file, 'r') as file:
            token_data = json.load(file)
    else:
        token_data = None

    # Attempt to acquire a token silently
    token = app.acquire_token_silent(scope, account=None)

    # If silent token acquisition failed, acquire a new token
    if not token:
        token = app.acquire_token_for_client(scopes=scope)

    # Print the token for debugging
    print("Token response:", token)

    # Save the token to a file for future use
    if token:
        with open(token_file, 'w') as file:
            json.dump(token, file)
        return token.get('access_token')
    else:
        print("Authentication failed.")
        return None

def create_easydraft():
    access_token = authenticate()
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }
    body = {
        'name': 'EasyDraft',
        'folder': {},
        '@microsoft.graph.conflictBehavior': 'rename'
    }
    response = requests.post(f'{API_ENDPOINT}/me/drive/root/children', json=body, headers=headers)
    if response.status_code == 201:
        print("EasyDraft folder created.")
    else:
        print(f"Error creating EasyDraft folder: {response.json()}")

def select_template(file_path):
    print("Function not implemented: select_template")

def download_template(file_name, local_directory):
    print("Function not implemented: download_template")

def upload_document(file_name, local_directory):
    print("Function not implemented: upload_document")

def delete_file(file_name, folder_name):
    print("Function not implemented: delete_file")

def delete_easydraft():
    print("Function not implemented: delete_easydraft")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python onedrive.py <function> [arguments]")
    else:
        function_name = sys.argv[1]
        args = sys.argv[2:]
        if function_name == 'create_easydraft':
            create_easydraft()
        elif function_name == 'select_template':
            if len(args) == 1:
                select_template(args[0])
            else:
                print("Usage: python onedrive.py select_template <file_path>")
        elif function_name == 'download_template':
            if len(args) == 2:
                download_template(args[0], args[1])
            else:
                print("Usage: python onedrive.py download_template <file_name> <local_directory>")
        elif function_name == 'upload_document':
            if len(args) == 2:
                upload_document(args[0], args[1])
            else:
                print("Usage: python onedrive.py upload_document <file_name> <local_directory>")
        elif function_name == 'delete_file':
            if len(args) == 2:
                delete_file(args[0], args[1])
            else:
                print("Usage: python onedrive.py delete_file <file_name> <folder_name>")
        elif function_name == 'delete_easydraft':
            delete_easydraft()
        else:
            print("Unknown function or incorrect number of arguments.")
