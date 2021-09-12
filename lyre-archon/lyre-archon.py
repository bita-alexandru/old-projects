from pyautogui import locateAllOnScreen, center, click
from pynput.mouse import Listener, Button
from json import load, dump

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

    setup_file: file = open(SETUP_PATH, "a+")

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

def handle_commands() -> str:
    print(">", end=" ")
    
    command: str = str(input()).replace(" ", "").lower()

    if command in ("exit", "quit", "q"):
        save_setup()
        return "EXIT"
    
    if command in ("save", "s"):
        save_setup()
        return "SAVE"
    
    if command in rows:
        setup_row(command)
        return "WHOLE_ROW"

    if len(command) == 3 and command[0] in "123" and command[1:3] in notes:
        num_row: int = int(command[0])
        row: str = rows[num_row-1]
        note: str = command[1:3]
        
        setup_note(row, note)
        return "SPECIFIC_NOTE"

    return "INVALID"

welcome_message = """Input your command:
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
