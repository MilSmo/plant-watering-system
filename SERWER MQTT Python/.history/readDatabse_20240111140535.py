import sqlite3


def read_database():
    conn = sqlite3.connect('mqtt_data.db')
    c = conn.cursor()

    c.execute("SELECT * FROM moisture")
    print(c.fetchall())

    c.execute("SELECT * FROM pump_actions")
    print(c.fetchall())

    conn.close()


if __name__ == "__main__":
    read_database()
    