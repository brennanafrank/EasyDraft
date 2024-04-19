# Kye Jocham

import os
import json
import requests
import sys
import logging
from pathlib import Path
from msal import PublicClientApplication
import webbrowser
import http.server
import socketserver
import urllib.parse

API_ENDPOINT = 'https://graph.microsoft.com/v1.0'

def authenticate():
    client_id = '3bf17d64-7891-4b2d-879c-a9a079222497'
    tenant_id = 'f8cdef31-a31e-4b4a-93e4-5f571e91255a'
    redirect_uri = 'http://localhost:8080'

    authority = 'https://login.microsoftonline.com/common'
    scope = ['https://graph.microsoft.com/.default']
    
    # Check if a valid token exists
    try:
        with open('token.json', 'r') as token_file:
            token_data = json.load(token_file)
            if token_data.get('access_token'):
                print('Using existing access token:', token_data['access_token'])
                return token_data['access_token']
    except FileNotFoundError:
        pass  # No token file found, proceed with authentication

    # Create a public client application
    app = PublicClientApplication(client_id, authority=authority)

    # Get the authorization URL
    auth_url = app.get_authorization_request_url(scopes=scope, redirect_uri=redirect_uri)
    
    # Open the authorization URL in the browser
    webbrowser.open(auth_url)
    
    # Start a simple HTTP server to listen for the redirect
    class RedirectHandler(http.server.BaseHTTPRequestHandler):
        def do_GET(self):
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b'Authentication successful. You can close this window.')
            # Parse the query parameters to get the authorization code
            query = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(query)
            self.server.auth_code = params.get('code', [None])[0]
    
    with socketserver.TCPServer(("", 8080), RedirectHandler) as httpd:
        print("Waiting for authentication...")
        httpd.handle_request()  # Handle a single request and then stop the server

    # Acquire a token using the authorization code
    if httpd.auth_code:
        result = app.acquire_token_by_authorization_code(code=httpd.auth_code, scopes=scope, redirect_uri=redirect_uri)
        if 'access_token' in result:
            access_token = result['access_token']
            print('Access token:', access_token)
            # Save the token to a file
            with open('token.json', 'w') as token_file:
                json.dump(result, token_file)
            return access_token
        else:
            error_description = result.get('error_description', 'Unknown error')
            print(f'Authentication failed: {error_description}')
            return None
    else:
        print('Authentication failed: No authorization code received')
        return None


def create_easydraft():
    # Authenticate user
    access_token = authenticate()

    # Define the base URL for the OneDrive API
    base_url = 'https://graph.microsoft.com/v1.0/me/drive/root'

    # Define the headers with the access token
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    # Check if the "EasyDraft" directory exists
    response = requests.get(f'{base_url}:/EasyDraft:', headers=headers)
    if response.status_code != 200:
        # Create the "EasyDraft" directory if it doesn't exist
        create_folder_payload = {
            'name': 'EasyDraft',
            'folder': {},
            '@microsoft.graph.conflictBehavior': 'fail'
        }
        create_response = requests.post(f'{base_url}/children', headers=headers, json=create_folder_payload)
        if create_response.status_code not in (200, 201):
            print('Error creating "EasyDraft" directory:', create_response.json())
            return
        else:
            print('"EasyDraft" directory created successfully.')
    else:
        print('"EasyDraft" directory already exists.')

    # Define the subdirectories to check and create
    subdirectories = ['Documents', 'Templates']

    # Check and create the subdirectories
    for subdir in subdirectories:
        response = requests.get(f'{base_url}:/EasyDraft/{subdir}:', headers=headers)
        if response.status_code != 200:
            # Create the subdirectory if it doesn't exist
            create_folder_payload = {
                'name': subdir,
                'folder': {},
                '@microsoft.graph.conflictBehavior': 'fail'
            }
            create_response = requests.post(f'{base_url}:/EasyDraft:/children', headers=headers, json=create_folder_payload)
            if create_response.status_code not in (200, 201):
                print(f'Error creating "{subdir}" directory:', create_response.json())
            else:
                print(f'"{subdir}" directory created successfully.')
        else:
            print(f'"{subdir}" directory already exists.')


def select_template(file_path):
    # Authenticate user
    access_token = authenticate()

    headers = {
        'Authorization': f'Bearer {access_token}',
    }

    # Extract the file name from the file_location
    file_name = os.path.basename(file_path)

    # Check if the file is already in the Templates directory
    search_url = f"{API_ENDPOINT}/me/drive/root:/EasyDraft/Templates/{file_name}:/content"
    try:
        response = requests.get(search_url, headers=headers)
        if response.status_code == 200:
            print(f"The file '{file_name}' is already in the Templates directory.")
            return
        elif response.status_code == 404:
            print(f"The file '{file_name}' is not in the Templates directory.")
        else:
            print(f"Failed to check the file '{file_name}'. Error: {response.text}")
            return
    except requests.exceptions.RequestException as e:
        print(f"Error while checking the file '{file_name}': {e}")
        return

    # If the file is not in the Templates directory, move it
    move_url = f"{API_ENDPOINT}/me/drive/root:{file_path}:/move"
    data = {
        "parentReference": {
            "path": "/drive/root:/EasyDraft/Templates"
        },
        "name": file_name
    }
    try:
        response = requests.patch(move_url, headers=headers, json=data)
        if response.status_code == 200:
            print(f"The file '{file_name}' has been moved to the Templates directory.")
        else:
            print(f"Failed to move the file '{file_name}'. Error: {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Error while moving the file '{file_name}': {e}")




"""
def select_template(file_path):
    # Authenticate user
    access_token = authenticate()

    base_url = 'https://graph.microsoft.com/v1.0/me/drive/root'
    headers = {'Authorization': f'Bearer {access_token}'}

    # Get the ID of the "Templates" folder
    templates_folder_response = requests.get(f'{base_url}:/EasyDraft/Templates:', headers=headers)
    if templates_folder_response.status_code != 200:
        print('Error retrieving "Templates" folder metadata:', templates_folder_response.json())
        return

    templates_folder_id = templates_folder_response.json()['id']

    # Get the file metadata from the specified path
    file_metadata_response = requests.get(f'{base_url}:{file_path}:', headers=headers)
    if file_metadata_response.status_code != 200:
        print('Error retrieving file metadata:', file_metadata_response.json())
        return

    file_metadata = file_metadata_response.json()
    file_id = file_metadata['id']
    file_name = file_metadata['name']

    # Check if the file already exists in the "Templates" folder
    check_response = requests.get(f'{base_url}:/EasyDraft/Templates/{file_name}:', headers=headers)
    if check_response.status_code == 200:
        print(f'File "{file_name}" already exists in "Templates" folder.')
        return

    # Define the payload for copying the file
    copy_payload = {
        'parentReference': {
            'id': templates_folder_id
        },
        'name': file_name
    }

    # Copy the file to the "Templates" folder
    copy_response = requests.post(f'{base_url}/items/{file_id}/copy', headers=headers, json=copy_payload)
    if copy_response.status_code != 202:
        print('Error copying file to "Templates" folder:', copy_response.json())
    else:
        print(f'File "{file_name}" copied to "Templates" folder successfully.')
"""

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
