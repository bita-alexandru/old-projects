notes_position = dict()

rows = ("first-row", "second-row", "third-row")
notes = ("do", "re", "me", "fa", "sol", "la", "si")

for row in rows:
    for note in notes:
        notes_positions[row][note] = (0, 0)

