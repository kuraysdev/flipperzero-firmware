import os
import re

translit = {
    'a': 'а', 'b': 'б', 'c': 'ц', 'd': 'д', 'e': 'е', 'f': 'ф', 'g': 'г',
    'h': 'х', 'i': 'и', 'j': 'ж', 'k': 'к', 'l': 'л', 'm': 'м', 'n': 'н',
    'o': 'о', 'p': 'п', 'q': 'ку', 'r': 'р', 's': 'с', 't': 'т', 'u': 'ю',
    'v': 'в', 'w': 'ш', 'x': 'х', 'y': 'й', 'z': 'з',
    'A': 'А', 'B': 'Б', 'C': 'Ц', 'D': 'Д', 'E': 'Е', 'F': 'Ф', 'G': 'Г',
    'H': 'Х', 'I': 'И', 'J': 'Ж', 'K': 'К', 'L': 'Л', 'M': 'М', 'N': 'Н',
    'O': 'О', 'P': 'П', 'Q': 'Ку', 'R': 'Р', 'S': 'С', 'T': 'Т', 'U': 'Ю',
    'V': 'В', 'W': 'Ш', 'X': 'Х', 'Y': 'Й', 'Z': 'З',
}

def transliterate(text):
    def repl(match):
        ch = match.group(0)
        return translit.get(ch, ch)
    return re.sub(r'[a-zA-Z]', repl, text)

def process_line(line):
    if "@" in line:
        return line
    ref_link_match = re.match(r'^(\s*)\[([^\]]+)\]:(\s*)(https?://\S+)(.*)$', line)
    if ref_link_match:
        prefix, key, ws, url, suffix = ref_link_match.groups()
        key_tr = transliterate(key)
        return f"{prefix}[{key_tr}]:{ws}{url}{suffix}\n" if not line.endswith("\n") else f"{prefix}[{key_tr}]:{ws}{url}{suffix}"
    pattern = r'(<[^>]+>|`[^`]+`|\[[^\]]+\]\([^)]+\)|https?://\S+)'
    parts = re.split(pattern, line)
    result = []
    for part in parts:
        if part is None or part == '':
            continue
        if re.match(pattern, part):
            result.append(part)
        else:
            result.append(transliterate(part))
    return ''.join(result)

def process_markdown(content):
    in_code_block = False
    result = []
    for line in content.splitlines(keepends=True):
        if line.strip().startswith("```"):
            in_code_block = not in_code_block
            result.append(line)
            continue
        if in_code_block:
            result.append(line)
            continue
        result.append(process_line(line))
    return ''.join(result)

for root, dirs, files in os.walk('.'):
    for fname in files:
        if fname.endswith('.md'):
            path = os.path.join(root, fname)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            new_content = process_markdown(content)
            with open(path, 'w', encoding='utf-8') as f:
                f.write(new_content)