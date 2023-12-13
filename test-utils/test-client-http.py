import requests

import requests

url = "http://127.0.0.1:8080"
headers = {'Content-Type': 'application/x-www-form-urlencoded'}
# data = {'Hello': 'world'}

res = requests.get(url, headers=headers)
