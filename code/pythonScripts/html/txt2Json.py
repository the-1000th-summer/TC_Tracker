f = open("haiyanTrack.txt","r")

out = "["

for line in f:
    lineInfo = line.split()

    day = int(lineInfo[2])
    hour = int(lineInfo[3])
    lat = float(lineInfo[4])
    lon = float(lineInfo[5])
    h_class = int(lineInfo[8])

    out += '{"day":%d, "hour":%d, "lat":%.1f, "lon":%.1f, "class": %d},' %(
        day, hour, lat, lon, h_class)
out = out[0:-1] + "]"
f.close()

with open('haiyanTrack.json', 'w') as outF:
    outF.write(out)
# print(out)
