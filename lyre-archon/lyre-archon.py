import mouse
import keyboard
import pyautogui
import ctypes
from json import load, dump
from random import randint

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

notes_mapping : dict = {
    "first-row": {
        note: key for note, key in zip(notes, "qwertyu")
    },
    "second-row": {
        note: key for note, key in zip(notes, "asdfghj")
    },
    "third-row": {
        note: key for note, key in zip(notes, "zxcvbnm")
    }
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

                notes_positions[row][note] = note_position

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
    x, y = mouse.get_position()

def reset_message() -> None:
    global message
    message = ""

def on_mouse_event(event) -> None:
    global message

    message = "INVALID"
    
    if isinstance(event, mouse.ButtonEvent):
        if event.event_type in (mouse.DOWN, mouse.DOUBLE):
            if event.button == mouse.LEFT:
                message = "LEFT"
                return
            if event.button == mouse.RIGHT:
                message = "RIGHT"
                return
        return

    if isinstance(event, mouse.WheelEvent):
        if event.delta == -1:
            message = "PREVIOUS"
            return
        if event.delta == 1:
            message = "NEXT"
        return

def setup_row(row: str) -> None:
    global x, y

    reset_xy()
    reset_message()

    note: str = notes[0]
    print("Setting \"%s\" on the \"%s\""%(note, row))

    mouse.hook(on_mouse_event)

    i: int = 0
    while i < NUM_NOTES:
        if message == "INVALID":
            continue

        if message == "PREVIOUS" and i > 0:
            i -= 1
            note = notes[i]

            reset_message()

            print("\nSwitched to note \"%s\""%note)
            continue

        if message == "NEXT" and i < NUM_NOTES - 1:
            i += 1
            note = notes[i]

            reset_message()

            print("\nSwitched to note \"%s\""%note)
            continue

        if message == "RIGHT":
            print("Operation canceled")
            break

        if message == "LEFT":
            x, y = mouse.get_position()
            note = notes[i]
            notes_positions[row][note] = (x, y)

            reset_message()

            print("Set \"%s\" on the \"%s\" with the coordinates (%i, %i)"%(note, row, x, y))

            i += 1
            if i < NUM_NOTES:
                note = notes[i]
                print("\nSetting \"%s\" on the \"%s\""%(note, row))
    
    mouse.unhook(on_mouse_event)

def setup_note(row: str, note: str) -> None:
    global x, y

    reset_xy()
    
    print("Setting \"%s\" on the \"%s\""%(note, row))
    
    mouse.hook(on_mouse_event)

    while True:
        if message == "INVALID":
            continue

        if message == "RIGHT":
            print("Operation canceled")
            return

        if message == "LEFT":
            x, y = mouse.get_position()
            notes_positions[row][note] = (x, y)

            reset_message()

            print("Set \"%s\" on the \"%s\" with the coordinates (%i, %i)"%(note, row, x, y))
            return

def play_note(row: str, note: str) -> None:
    # RADIUS = 10
    # x, y = notes_positions[row][note]
    
    # dx: int = randint(-RADIUS, RADIUS)
    # dy: int = randint(-RADIUS, RADIUS)

    # x = max(0, x+dx)
    # x = min(WIDTH, x)

    # y = max(0, y+dy)
    # y = min(HEIGHT, y)

    # mouse.move(x, y)
    # mouse.click(mouse.LEFT)

    key: str = notes_mapping[row][note]
    keyboard.send(key)

def play_song():
    SONG_PATH: str = "song.txt"
    
    open(SONG_PATH, "at").close()
    song_file: file = open(SONG_PATH, "rt")

    reset_message()

    tabs: tuple = song_file.read().replace("\n", " ").replace("\t", " ").split(" ")

    i: int = 0
    while i < len(tabs):
        tab: str = tabs[i]

        method: str = "INVALID"
        if len(tab) == 1 and tab[0] in "qwertyuasdfghjzxcvbnm":
            method = "KEY"
        elif len(tab) == 3 and tab[0] in "123" and tab[1:3] in notes:
            method = "NOTE"
        
        if method == "INVALID":
            i += 1
            continue

        key_event: keyboard.KeyboardEvent = keyboard.read_event()

        if key_event.event_type != keyboard.KEY_DOWN:
            continue

        key_name = key_event.name

        if "shift" in key_name:
            i += 1

            if method == "KEY":
                keyboard.send(tab)
            elif method == "NOTE":
                row_num = int(tab[0])
                row: str = rows[row_num - 1]
                note: str = tab[1:3]

                play_note(row, note)
            
            print(tab, end=" ", flush=True)
            continue

        if key_name == "esc":
            print("Operation canceled")
            return
            
    print()

def handle_commands() -> str:
    print(">", end=" ")
    
    command: str = str(input()).replace(" ", "").replace("\t", "").lower()

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
        for row in rows: 
            setup_row(row)
            
            if message == "RIGHT":
                break

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
