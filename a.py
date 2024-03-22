from atlasapi.atlas import Atlas

a = Atlas("turmmrng","84df3787-314b-409f-a022-8ee40ee80269","65f78532fa810d6773ffe85e")

# Low level Api
details = a.DatabaseUsers.get_all_database_users(pageNum=1, itemsPerPage=100)

# Iterable
for user in a.DatabaseUsers.get_all_database_users(iterable=True):
    print(user["username"])

