import requests
import concurrent
from concurrent.futures import ThreadPoolExecutor
characters = range(1, 100)
base_url = 'https://192.168.1.140:8080'
threads = 20
def get_character_info(character):
    r = requests.get(f'{base_url}')
    return r.json()
with ThreadPoolExecutor(max_workers=threads) as executor:
    executor.submit(get_character_info)
