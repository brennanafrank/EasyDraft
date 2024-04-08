import os
import json
import sys
import base64
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.auth.exceptions import RefreshError
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders

SCOPES = ['https://www.googleapis.com/auth/gmail.compose']

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
            try:
                creds.refresh(Request())
            except RefreshError:
                # If the refresh token is invalid, delete the token file and start the flow again.
                os.remove('token.json')
                flow = InstalledAppFlow.from_client_secrets_file(
                    'credentials.json', SCOPES)
                creds = flow.run_local_server(port=0)
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


def send_document(recipient, subject, message_text, file_location):
    try:
        # Check if the recipient email is valid (simple check)
        if '@' not in recipient or '.' not in recipient:
            raise ValueError("Invalid recipient email address.")

        # Check if the file exists
        if not os.path.exists(file_location):
            raise FileNotFoundError("The specified file does not exist.")

        # Create a MIME message
        message = MIMEMultipart()
        message['to'] = recipient
        message['subject'] = subject

        # Attach the message text
        msg = MIMEText(message_text)
        message.attach(msg)

        # Attach the file
        part = MIMEBase('application', 'octet-stream')
        with open(file_location, 'rb') as file:
            part.set_payload(file.read())
        encoders.encode_base64(part)
        part.add_header('Content-Disposition', f'attachment; filename="{os.path.basename(file_location)}"')
        message.attach(part)

        # Encode the message as base64
        raw_message = base64.urlsafe_b64encode(message.as_bytes()).decode()

        # Send the message
        service = build('gmail', 'v1', credentials=authenticate())
        message = {'raw': raw_message}
        try:
            message = (service.users().messages().send(userId='me', body=message).execute())
            print(f"Message Id: {message['id']}\nMessage sent successfully to {recipient}")
        except HttpError as error:
            print(f"An error occurred: {error}")

    except ValueError as e:
        print(f"ValueError: {e}")
    except FileNotFoundError as e:
        print(f"FileNotFoundError: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


if __name__ == '__main__':
    num_args = len(sys.argv)
    if num_args in [0, 1]:
        print("Usage: python gmail.py <function> [arguments]")
    elif num_args == 2:  # No arguments for the function
        function_name = sys.argv[1]
        if function_name == 'authenticate':
            authenticate()
        else:
            print("Unknown function name or incorrect number of arguments.")
    elif num_args == 6:  # Arguments for the send_document function
        function_name = sys.argv[1]
        if function_name == 'send_document':
            recipient = sys.argv[2]
            subject = sys.argv[3]
            message_text = sys.argv[4]
            file_location = sys.argv[5]
            send_document(recipient, subject, message_text, file_location)
        else:
            print("Unknown function name or incorrect number of arguments.")
    else:
        print("Incorrect number of arguments.")
