import pyautogui
from pynput.mouse import Listener, Button
from pynput import keyboard
from json import load, dump
from random import randint
import ctypes

USER32 = ctypes.windll.user32
WIDTH, HEIGHT = USER32.GetSystemMetrics(0), USER32.GetSystemMetrics(1)

pyautogui.FAILSAFE = False

SETUP_PATH: str = "setup.json"

rows: tuple = ("first-row", "second-row", "third-row")
notes: tuple = ("do", "re", "mi", "fa", "so", "la", "si")

NUM_ROWS: int = 3
NUM_NOTES: int = 7

notes_positions: dict = {
    row: {
        note: (0, 0) for note in notes
    } for row in rows
}

x: int = -1
y: int = -1
message: str = ""

def load_setup() -> bool:
    global notes_positions

    open(SETUP_PATH, "at").close()
    setup_file: file = open(SETUP_PATH, "rt")

    print("Loading setup...")

    try:
        setup_dict: dict = load(setup_file)
        
        if tuple(setup_dict.keys()) != rows: throw()
        
        for row in setup_dict.keys():
            row_dict: dict = setup_dict[row]
            
            if tuple(row_dict.keys()) != notes: throw()
            
            for note in row_dict.keys():
                note_position: tuple = row_dict[note]
                
                if len(note_position) != 2: throw()
                if not isinstance(note_position[0], int) or not isinstance(note_position[1], int): throw()

        print("Setup loaded.\n")
        return True
    except:
        print("Setup not loaded.\n")
        return False

def save_setup() -> bool:
    setup_file: file = open(SETUP_PATH, "wt")

    print("Saving setup...")

    try:
        dump(notes_positions, setup_file, indent=2)

        print("Setup saved.\n")
        return True
    except:
        print("Setup not saved.\n")
        return False

def view_setup() -> None:
    print("Viewing setup...")
    print(notes_positions)
    print()

def reset_xy() -> None:
    global x, y
    global message

    x, y = -1, -1
    message = ""

def on_click(_x: int, _y: int, button: Button, pressed: int) -> bool:
    global x, y
    global message

    x, y = _x, _y

    if button == Button.left and not pressed:
        message = "LEFT"
        return False
    
    if button == Button.right:
        message = "RIGHT"
        return False
    
    message = "INVALID"
    return False

def on_scroll(_x: int, _y: int, dx: int, dy: int) -> bool:
    global message

    if dy == -1:
        message = "PREVIOUS"
        return False

    if dy == 1:
        message = "NEXT"
        return False
    
    return False

def on_press(key: keyboard.Key) -> bool:
    global message

    if key in (keyboard.Key.ctrl_r, keyboard.Key.ctrl_l):
        message = "CTRL"
        return False
    
    if key == keyboard.Key.esc:
        message = "ESC"
        return False
    
    message = "INVALID"
    return False

def setup_row(row: str) -> None:
    i: int = 0
    while i < NUM_NOTES:
        note: str = notes[i]

        if message in ("", "LEFT"):
            print("Setting \"%s\" on the \"%s\""%(note, row))
            
        reset_xy()

        with Listener(on_click=on_click, on_scroll=on_scroll) as listener:
            listener.join()
        
        if message == "PREVIOUS" and i > 0:
            i -= 1
            note = notes[i]

            print("\nSwitched to note \"%s\""%note)
            continue

        if message == "NEXT" and i < NUM_NOTES - 1:
            i += 1
            note = notes[i]

            print("\nSwitched to note \"%s\""%note)
            continue
        
        if message in "RIGHT":
            print("Operation canceled")
            break

        if message == "LEFT":
            note = notes[i]
            notes_positions[row][note] = (x, y)

            print("Set \"%s\" on the \"%s\" with the coordinates (%i, %i)"%(note, row, x, y))

            i += 1
        
        if message in ("", "LEFT"):
            print()

def setup_note(row: str, note: str) -> None:
    while True:
        if message == "":
            print("Setting \"%s\" on the \"%s\""%(note, row))
            
        reset_xy()

        with Listener(on_click=on_click) as listener:
            listener.join()

            if message in "RIGHT":
                print("Operation canceled")
                return

            if message == "LEFT":
                notes_positions[row][note] = (x, y)

                print("Set \"%s\" on the \"%s\" with the coordinates (%i, %i)"%(note, row, x, y))
                return

def play_note(row: str, note: str) -> None:
    RADIUS = 10
    x, y = notes_positions[row][note]
    
    dx: int = randint(-RADIUS, RADIUS)
    dy: int = randint(-RADIUS, RADIUS)

    x = max(0, x+dx)
    x = min(WIDTH, x)

    y = max(0, y+dy)
    y = min(HEIGHT, y)

    pyautogui.click(x, y)

def play_song():
    SONG_PATH: str = "song.txt"
    open(SONG_PATH, "at").close()
    song_file: file = open(SONG_PATH, "rt")

    tabs: tuple = song_file.read().replace("\n", "").split(" ")
    i: int = 0
    while i < len(tabs):
        tab: str = tabs[i]

        if len(tab) != 3 or tab[0] not in "123" or tab[1:3] not in notes:
            i += 1
            continue

        reset_xy()

        with keyboard.Listener(on_press=on_press) as listener:
            listener.join()

        if message == "CTRL":
            i += 1

            row_num = int(tab[0])
            row: str = rows[row_num - 1]
            note: str = tab[1:3]

            play_note(row, note)
            
            print(tab, end=" ", flush=True)
            continue

        if message == "ESC":
            print("Operation canceled")
            return
            
    print()

def handle_commands() -> str:
    print(">", end=" ")
    
    command: str = str(input()).replace(" ", "").lower()

    if command in ("exit", "quit", "q"):
        save_setup()
        return "EXIT"
    
    if command in ("save", "s"):
        save_setup()
        return "SAVE"

    if command == "play":
        play_song()
        return "PLAY"

    if command == "full":
        for row in rows: setup_row(row)
        return "FULL"
    
    if command in rows:
        setup_row(command)
        return "ROW"

    if len(command) == 3 and command[0] in "123" and command[1:3] in notes:
        num_row: int = int(command[0])
        row: str = rows[num_row-1]
        note: str = command[1:3]
        
        setup_note(row, note)
        return "NOTE"

    return "INVALID"

welcome_message = """Input your command:
"play": to start playing your song,
"full": to setup the positions of the notes on all of the rows,
"first-row": to setup the positions of the notes on the first row,
"second-row": to setup the positions of the notes on the second row,
"third-row": to setup the positions of the notes on the third row,
["1" | "2" | "3"] ["do" | "re" | "mi"| "fa" | "so" | "la" | "si"]: to setup a specific note,
"save" | "s": to save the current setup,
"exit" | "quit" | "q": to quit the program."""

if __name__ == "__main__":
    load_setup()
    view_setup()

    print(welcome_message)

    while True:
        reset_xy()
        result = handle_commands()

        if result == "EXIT":
            break
