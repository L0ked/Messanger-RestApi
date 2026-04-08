# scripts/seed.py
import psycopg
import random
from faker import Faker
from datetime import datetime, timedelta

fake = Faker()

DB = "dbname=MessengerTest user=postgres password=Wowa+ host=localhost"

NUM_USERS = 50
NUM_GROUP_CHATS = 15
NUM_PERSONAL_CHATS = 25
MESSAGES_PER_CHAT = (5, 40)
import nacl.pwhash
TEST_PASSWORD_HASH = nacl.pwhash.argon2id.str(b"test123").decode()

def main():
    with psycopg.connect(DB) as conn:
        with conn.cursor() as cur:
            # чистим в правильном порядке (из-за FK)
            cur.execute("truncate messages, chat_members, chats, sessions, users restart identity cascade")

            # users
            user_ids = []
            for _ in range(NUM_USERS):
                username = fake.unique.user_name()
                cur.execute(
                    "insert into users(username, password, display_name) "
                    "values(%s, %s, %s) returning id",
                    (username, TEST_PASSWORD_HASH, fake.name())
                )
                user_ids.append(cur.fetchone()[0])
            print(f"Inserted {len(user_ids)} users")

            # personal chats
            personal_count = 0
            used_pairs = set()
            for _ in range(NUM_PERSONAL_CHATS):
                u1, u2 = random.sample(user_ids, 2)
                pair = tuple(sorted((u1, u2)))
                if pair in used_pairs:
                    continue
                used_pairs.add(pair)

                cur.execute(
                    "insert into chats(name, description, type) values('', '', 1) returning id"
                )
                chat_id = cur.fetchone()[0]
                cur.execute(
                    "insert into chat_members(chat_id, user_id, role) values(%s, %s, 1), (%s, %s, 1)",
                    (chat_id, u1, chat_id, u2)
                )
                personal_count += 1
                _fill_messages(cur, chat_id, [u1, u2])
            print(f"Inserted {personal_count} personal chats")

            # group chats
            for _ in range(NUM_GROUP_CHATS):
                cur.execute(
                    "insert into chats(name, description, type) values(%s, %s, 2) returning id",
                    (fake.catch_phrase()[:128], fake.sentence()[:256])
                )
                chat_id = cur.fetchone()[0]
                members = random.sample(user_ids, random.randint(3, 8))
                for i, uid in enumerate(members):
                    role = 1 if i == 0 else 0
                    cur.execute(
                        "insert into chat_members(chat_id, user_id, role) values(%s, %s, %s)",
                        (chat_id, uid, role)
                    )
                _fill_messages(cur, chat_id, members)
            print(f"Inserted {NUM_GROUP_CHATS} group chats")

            cur.execute("select count(*) from messages")
            print(f"Total messages: {cur.fetchone()[0]}")

        conn.commit()

def _fill_messages(cur, chat_id, members):
    n = random.randint(*MESSAGES_PER_CHAT)
    base = datetime.now() - timedelta(days=30)
    for i in range(n):
        uid = random.choice(members)
        ts = base + timedelta(minutes=i * random.randint(1, 60))
        cur.execute(
            "insert into messages(chat_id, user_id, content, created_at) "
            "values(%s, %s, %s, %s)",
            (chat_id, uid, fake.text(max_nb_chars=200), ts)
        )

if __name__ == "__main__":
    main()