import cv2
from pyautogui import screenshot, click, locateOnScreen, ImageNotFoundException
from time import sleep
from random import randint, uniform
import pytesseract
from PIL import Image
from numpy import array
from cv2 import cvtColor, imwrite, COLOR_RGB2BGR

# x1, y1, x2, y2

BTN_CELLPHONE = (
    1785, 895,
    1854, 959
)

BTN_PROMOTE = (
    1695, 913,
    1749, 958
)

BTN_CANWATCH = (
    1520, 636,
    1687, 659
)

BTN_DONTUSE = (
    1446, 617,
    1579, 639
)

TXT_REWARDIN_UPRIGHT = (
    1611, 50,
    1818-1611, 88-50
)

TXT_REWARDIN_MIDDLE = (
    1335, 678,
    1484-1335, 702-678
)

TXT_REWARDIN_DOWN = (
    1374, 968,
    1525-1374, 993-968
)

TXT_REWARDIN_UPLEFT = (
    1391, 53,
    1574-1391, 90-53
)

BTN_BACK = (
    1900, 909,
    1900, 909
)

BTN_XFACEBOOK = (
    1849, 54,
    1849, 54
)

BTN_WISHINGWELL = (
    1591, 713,
    1638, 752
)

BTN_YES = (
    1544, 644,
    1673, 663
)

BTN_STAGE = (
    1642, 649,
    1662, 714
)

BTN_PLUS = (
    1520, 795,
    1543, 809
)

BTN_OBTAIN = (
    1625, 446,
    1766, 577
)

BTN_CALLBACK = (
    1623, 191,
    1803, 214
)

BTN_MAP = (
    1371, 213,
    1395, 239
)

MAP_POND = (
    1440, 464,
    1486, 507
)

MAP_GARDEN = (
    1479, 563,
    1515, 604
)

MAP_RESTAURANT = (
    1575, 478,
    1627, 528
)

MAP_KITCHEN = (
    1696, 537,
    1732, 575
)

MAP_BUFFET = (
    1605, 597,
    1652, 646
)

MAP_COURTYARD = (
    1534, 374,
    1619, 416
)

MAP_TAKEOUT = (
    1719, 441,
    1754, 477
)

BTN_EXPERIENCE = (
    1510, 783,
    1586, 821
)

BTN_BAIT = (
    1608, 143,
    1627, 161
)

BTN_POND = (
    1568, 490,
    1663, 519
)

BTN_LEFT = (
    1360, 510,
    1368, 522
)

BTN_RIGHT = (
    1846, 509,
    1853, 521
)

BTN_GIVEITATRY = () # magicianul

def get_xy(btn):
    x1, y1, x2, y2 = btn

    x = randint(x1, x2)
    y = randint(y1, y2)

    return x, y

def tap(btn, seconds=0):
    x, y = get_xy(btn)

    click(x, y)
    sleep(seconds)

def get_words(txt):
    reward = screenshot(region=txt)
    reward = cvtColor(array(reward), COLOR_RGB2BGR)

    imwrite("reward.png", reward)

    text = pytesseract.image_to_string(Image.open("reward.png")).replace("\x0c", "").replace("\n", "")

    return text.split(" ")

def teleport(location):
    tap(BTN_MAP, 0.5)

    tap(location, 5)
    
    #tap(BTN_BACK)

def get_seconds(words):
    for i in range(len(words)):
        word = words[i]
        
        try:
            seconds = float(word)
            if "second" in words[i+1]:
                return seconds
        except Exception as e:
            pass
    
    return -1

def watch_ad():
    tap(BTN_DONTUSE, 1.0)

    no_words = False

    while True:
        words = get_words(TXT_REWARDIN_UPRIGHT)
        seconds = get_seconds(words)
        
        if seconds != -1:
            break

        words = get_words(TXT_REWARDIN_MIDDLE)
        seconds = get_seconds(words)
        
        if seconds != -1:
            break

        words = get_words(TXT_REWARDIN_DOWN)
        seconds = get_seconds(words)
        if seconds != -1:
            break
            
        words = get_words(TXT_REWARDIN_UPLEFT)
        seconds = get_seconds(words)
        
        if seconds != -1:
            break
        
        no_words = True
        seconds = 46
        break
    
    print(words)

    sleep(seconds)

    if no_words:
        #tap(BTN_BACK, 0.1)
        #tap(BTN_BACK)
        tap(BTN_XFACEBOOK, 3)
        tap(BTN_XFACEBOOK)
    else:
        tap(BTN_BACK)

def promote_15x():
    teleport(MAP_RESTAURANT);

    for i in range(36):
        while True:
            tap(BTN_PROMOTE, 0.5)

            if locateOnScreen("full-queue-15.png", confidence=0.8) is None:
                break
            else:
                tap(BTN_BACK, 15)

        tap(BTN_CANWATCH, 0.5)
        
        print("Ad No. %i"%(i+1))
        watch_ad()

        sleep(46)

def promote_1x():
    teleport(MAP_RESTAURANT)

    COOLDOWN_MAX = 0.15
    COOLDOWN_MIN = 0.12

    INVITES_MAX = 15
    INVITES_MIN = 5

    PAUSE_MAX = 30
    PAUSE_MIN = 10

    while True:
        invites = randint(INVITES_MIN, INVITES_MAX)

        print("invites: %i"%invites)

        for i in range(invites):
            print("no.%i"%i)
            cooldown = uniform(COOLDOWN_MIN, COOLDOWN_MAX)
            tap(BTN_CELLPHONE, cooldown)

            if locateOnScreen("full-queue-1.png", confidence=0.8) != None:
                break

            for j in range(30):
                cooldown = uniform(COOLDOWN_MIN, COOLDOWN_MAX)
                tap(BTN_CELLPHONE, cooldown)
        
        pause = randint(PAUSE_MIN, PAUSE_MAX)

        print("pause: %f"%pause)

        sleep(pause)


def wish():
    teleport(MAP_GARDEN)

    for i in range(36):
        tap(BTN_WISHINGWELL, 0.5)
        tap(BTN_YES, 0.5)
        print("Ad No. %i"%(i+1))
        watch_ad()
        sleep(30)

def buy_invites():
    teleport(MAP_COURTYARD)
    
    tap(BTN_LEFT, 1)
    tap(BTN_STAGE, 0.5)
    tap(BTN_PLUS, 0.5)
    
    for i in range(5):
        tap(BTN_OBTAIN, 0.5)
        print("Ad No. %i"%(i+1))
        watch_ad()
        sleep(5)
    
    tap(BTN_BACK)
    tap(BTN_BACK)

def buy_callbacks():
    #teleport(MAP_COURTYARD)
    
    #tap(BTN_LEFT, 1)
    tap(BTN_STAGE, 0.5)
    tap(BTN_CALLBACK, 0.5)
    tap(BTN_PLUS, 0.5)

    for i in range(5):
        tap(BTN_OBTAIN, 0.5)
        print("Ad No. %i"%(i+1))
        watch_ad()
        sleep(5)
        
    tap(BTN_BACK)
    tap(BTN_BACK)

def buy_bait():
    teleport(MAP_POND)
    
    tap(BTN_POND, 0.5)
    tap(BTN_BAIT, 0.5)
    tap(BTN_BACK, 0.5)
    
    for i in range(20):
        tap(BTN_OBTAIN, 0.5)
        print("Ad No. %i"%(i+1))
        watch_ad()
        sleep(5)
    
    tap(BTN_BACK)
    tap(BTN_BACK)

def get_experience(times):
    for i in range(times):
        tap(BTN_EXPERIENCE, 0.5)
        
        watch_ad()
        
        sleep(5)

if __name__ == "__main__":
    pytesseract.pytesseract.tesseract_cmd = "D:\\Utilitare\\Tesseract\\tesseract.exe"

    sleep(5)

    #buy_invites()
    #buy_callbacks()
    buy_bait()
    wish()
    promote_15x()
    promote_1x()
    #get_experience(13)