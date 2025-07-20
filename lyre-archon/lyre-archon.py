import mido
import ctypes
import keyboard
from time import sleep
from random import randint
from json import load, dump

USER32 = ctypes.windll.user32
WIDTH, HEIGHT = USER32.GetSystemMetrics(0), USER32.GetSystemMetrics(1)

SETUP_PATH: str = "setup.json"

rows: tuple = ("first-row", "second-row", "third-row")
notes: tuple = ("do", "re", "mi", "fa", "so", "la", "si")

key_map: dict = {
    rows[0]: "QWERTYU",
    rows[1]: "ASDFGHJ",
    rows[2]: "ZXCVBNM"
}

note_map: dict = dict()

transform = "IGNORE" # "SHARP" / "FLAT"

def load_setup() -> bool:
    global notes_positions

    open(SETUP_PATH, "at").close()
    setup_file: file = open(SETUP_PATH, "rt")

    print("Loading setup...")

    try:
        setup_dict: dict = load(setup_file)
        
        if tuple(setup_dict.keys()) != rows: throw()

        keys_used: str = ""
        
        for row in setup_dict.keys():
            keys: str = setup_dict[row]
            
            if len(keys) != 7: throw()
            
            for key in keys:
                key = key.upper()

                if key in keys_used: throw()
                
                keys_used += key

            key_map[row] = keys

        print("Setup loaded.\n")
        return True
    except:
        print("Setup not loaded.\n")
        return False

def save_setup() -> bool:
    setup_file: file = open(SETUP_PATH, "wt")

    print("Saving setup...")

    try:
        dump(key_map, setup_file, indent=2)

        print("Setup saved.\n")
        return True
    except:
        print("Setup not saved.\n")
        return False

def view_setup() -> None:
    print("Viewing setup...")
    print(key_map)
    print()

def setup() -> None:
    global key_map

    keys_used: str = ""
    backup: dict = key_map

    for row in rows:
        note: str = notes[0]
        print("Setting \"%s\" on the \"%s\""%(note, row))

        i: int = 0
        while i < 7:
            key_event: keyboard.KeyboardEvent = keyboard.read_event()

            if key_event.event_type != keyboard.KEY_UP:
                continue

            key_name = key_event.name.upper()

            if key_name == "LEFT" and i > 0:
                i -= 1
                note = notes[i]

                print("\nSwitched to note \"%s\""%note)
                continue

            if key_name == "RIGHT" and i < 7 - 1:
                i += 1
                note = notes[i]

                print("\nSwitched to note \"%s\""%note)
                continue

            if key_name == "ESC":
                key_map = backup

                print("Operation canceled")
                return

            if len(key_name) == 1:
                note = notes[i]

                if key_name in keys_used:
                    print("This key is already being used.")
                    continue

                keys_used += key_name

                tmp: list = list(key_map[row])
                tmp[i] = key_name

                key_map[row] = "".join(tmp)

                print("Set \"%s\" on the \"%s\" with the key %s"%(note, row, key_name))

                i += 1
                if i < 7:
                    note = notes[i]
                    print("\nSetting \"%s\" on the \"%s\""%(note, row))
    
        print()

def play_script() -> None:
    SCRIPT_PATH: str = "song.txt"
    
    open(SCRIPT_PATH, "at").close()
    song_file: file = open(SCRIPT_PATH, "rt")

    tabs: tuple = song_file.read().replace("\n", " ").replace("\t", " ").split(" ")

    i: int = 0
    while i < len(tabs):
        tab: str = tabs[i].lower()

        keys: str = key_map[rows[0]] + key_map[rows[1]] + key_map[rows[2]]
        
        if len(tab) != 1 or tab.upper() not in keys:
            i += 1
            continue

        key_event: keyboard.KeyboardEvent = keyboard.read_event()

        if key_event.event_type != keyboard.KEY_DOWN:
            continue

        key_name = key_event.name

        if "shift" in key_name:
            i += 1

            keyboard.send(tab)
            
            print(tab, end=" ", flush=True)
            continue

        if key_name == "esc":
            print("Operation canceled")
            return
            
    print()

def get_key_from_note(note: int) -> str:
    while True:
        if note > 83:
            note -= 12
        elif note < 48:
            note += 12
        break

    if note not in note_map:
        if transform == "IGNORE":
            return ""
        
        if transform == "SHARP":
            note += 1
        elif transform == "FLAT":
            note -= 1
    
    return note_map[note]

def play_midi() -> None:
    MIDI_FILE = "song.mid"

    try:
        mid: mido.MidiFile = mido.MidiFile(MIDI_FILE)

        sleep(3)

        for msg in mid.play():
            if msg.type != "note_on":
                continue

            key: str = get_key_from_note(msg.note)

            if len(key) == 0:
                continue

            keyboard.send(key.lower())

            print(key, end=" ", flush=True)
        
        print()
        
    except FileNotFoundError:
        print("File not found.")
    except Exception as e:
        print("File is not a valid MIDI.")

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
        # play_script()
        play_midi()
        return "PLAY"

    if command == "setup":
        setup()

        return "FULL"
    
    return "INVALID"

def welcome() -> None:
    welcome_message = """Input your command:
    "play": to start playing your song,
    "setup": to setup the keys of the notes,
    "save" / "s": to save the current setup,
    "exit" / "quit" / "q": to quit the program."""

    print(welcome_message)

def reset_key_map() -> None:
    global key_map

    key_map = {
        rows[0]: "qwertyu",
        rows[1]: "asdfghj",
        rows[2]: "zxcvbnm"
    }

def init_note_map() -> None:
    global note_map

    keys: tuple = (
    #   DO, RE, MI, FA, SO, LA, SI
        72, 74, 76, 77, 79, 81, 83,
        60, 62, 64, 65, 67, 69, 71,
        48, 50, 52, 53, 55, 57, 59
        )

    for i_row in range(3):
        for i_key in range(7):
            key: int = keys[i_row*7 + i_key]
            row: str = rows[i_row]

            note_map[key] = key_map[row][i_key]

if __name__ == "__main__":
    if not load_setup():
        reset_key_map()

    view_setup()

    init_note_map()

    welcome()

    while True:
        result = handle_commands()

        if result == "EXIT":
            break
