import json

lt = []
start = 0

with open("yaoce.txt", 'r', encoding='utf8') as f:
    while 1:
        s = f.readline()
        if s == '':
            break
        m1 = {
            "type": "value",
            "name": s,
            "length": 8
        }
        m2 = {
            "type": "map",
            "name": s,
            "length": 1,
            "map": {
                "0": "通过",
                "1": "未通过"
            }
        }
        m3 = {
            "param_Name": s,
            "param_Type": 1,
            "param_StartBit": start,
            "param_AnalysisBits": 1
        }
        lt.append(m3)
        start += 1


sp = json.dumps(lt, ensure_ascii=False)
sp = sp.replace(r"\n", '')

with open("yaoce_py.json", 'w', encoding="utf8") as f:
    f.write(sp)
