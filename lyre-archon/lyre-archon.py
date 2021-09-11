from pyautogui import locateAllOnScreen, center, click
from pynput.mouse import Listener

def on_click(x, y, button, pressed):
    print(x, y, button, pressed)
    return False

def setup_row(row):
    with Listener(on_click=on_click) as listener:
        listener.join()

def setup_note(row, note):
    with Listener(on_click=on_click) as listener:
        listener.join()

def handle_commands():
    print(">", end=" ")
    
    command = str(input()).replace(" ", "").lower()

    if command in ("exit", "quit", "q"):
        return "EXIT"
    
    if command in rows:
        setup_row(command)
        return "WHOLE_ROW"

    if len(command) == 3 and command[0] in "123" and command[1:3] in notes:
        num_row = int(command[0])
        row = rows[num_row-1]
        note = command[1:3]

        setup_note(row, note)
        return "SPECIFIC_NOTE"

    return "INVALID"

rows = ("first-row", "second-row", "third-row")
notes = ("do", "re", "me", "fa", "so", "la", "si")

notes_positions = {
    row: {
        note: (0, 0) for note in notes
    } for row in rows
}

welcome_message = """Input your command:
"first-row": to setup the positions of the notes on the first row,
"second-row": to setup the positions of the notes on the second row,
"third-row": to setup the positions of the notes on the third row,
["1" | "2" | "3"] ["do" | "re" | "mi"| "fa" | "so" | "la" | "si"]: to setup a specific note,
"exit" | "quit" | "q": to quit the program."""

print(welcome_message)

while True:
    result = handle_commands()

    if result == "EXIT":
        break

print("gata")
