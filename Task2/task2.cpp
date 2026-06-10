#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <random>
#include <algorithm>

using namespace std; // <-- добавлено

const int Nb = 4;
const int Nk = 4;
const int Nr = 10;
const size_t BLOCK_SIZE = 16;
const string KEY_FILE = "aes_key.bin";

// Нелинейная таблица замен (S-Box)
const unsigned char s_box[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Инвертированный S-Box (для дешифрования)
const unsigned char inv_s_box[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// Константы Rcon[] для Key Expansion, каждый последующий - умножение предыдущего на 0x02 в поле Галуа
const unsigned char Rcon[11] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

string to_hex(const unsigned char* data, size_t size) {
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < size; ++i) ss << setw(2) << static_cast<int>(data[i]);
    return ss.str();
}

void print_state(const unsigned char state[4][4], const string& msg) {
    cout << "State [" << msg << "]: ";
    for (int c = 0; c < Nb; ++c)
        for (int r = 0; r < 4; ++r)
            cout << hex << setw(2) << setfill('0') << static_cast<int>(state[r][c]);
    cout << dec << "\n";
}

void BytesToMatrix(const unsigned char* input, unsigned char state[4][4]) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb; ++c)
            state[r][c] = input[c * 4 + r];
}

void MatrixToBytes(const unsigned char state[4][4], unsigned char* output) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb; ++c)
            output[c * 4 + r] = state[r][c];
}

void expand_key(const unsigned char* cipher_key, unsigned char* round_keys) {
    for (int i = 0; i < Nk * 4; ++i)
        round_keys[i] = cipher_key[i];

    unsigned char tmp[4];
    for (int i = Nk; i < Nb * (Nr + 1); ++i) {
        for (int j = 0; j < 4; ++j) tmp[j] = round_keys[(i - 1) * 4 + j];
        if (i % Nk == 0) {
            unsigned char k = tmp[0];
            tmp[0] = tmp[1]; tmp[1] = tmp[2]; tmp[2] = tmp[3]; tmp[3] = k;
            tmp[0] = s_box[tmp[0]]; tmp[1] = s_box[tmp[1]];
            tmp[2] = s_box[tmp[2]]; tmp[3] = s_box[tmp[3]];
            tmp[0] ^= Rcon[i / Nk];
        }
        for (int j = 0; j < 4; ++j)
            round_keys[i * 4 + j] = round_keys[(i - Nk) * 4 + j] ^ tmp[j];
    }
}

void add_round_key(unsigned char state[4][4], const unsigned char* round_keys, int round) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb; ++c)
            state[r][c] ^= round_keys[round * Nb * 4 + c * 4 + r];
}

void sub_bytes(unsigned char state[4][4]) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb; ++c)
            state[r][c] = s_box[state[r][c]];
}

void inv_sub_bytes(unsigned char state[4][4]) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb; ++c)
            state[r][c] = inv_s_box[state[r][c]];
}

void shift_rows(unsigned char state[4][4]) {
    unsigned char tmp[4];
    for (int r = 1; r < 4; ++r) {
        for (int c = 0; c < Nb; ++c) tmp[c] = state[r][(c + r) % Nb];
        for (int c = 0; c < Nb; ++c) state[r][c] = tmp[c];
    }
}

void inv_shift_rows(unsigned char state[4][4]) {
    unsigned char tmp[4];
    for (int r = 1; r < 4; ++r) {
        for (int c = 0; c < Nb; ++c) tmp[c] = state[r][(c + Nb - r) % Nb];
        for (int c = 0; c < Nb; ++c) state[r][c] = tmp[c];
    }
}

unsigned char galois_multiply(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    for (int counter = 0; counter < 8; counter++) {
        if (b & 1) p ^= a;
        bool hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

void mix_columns(unsigned char state[4][4]) {
    unsigned char tmp[4];
    for (int c = 0; c < Nb; ++c) {
        tmp[0] = galois_multiply(state[0][c], 2) ^ galois_multiply(state[1][c], 3) ^ state[2][c] ^ state[3][c];
        tmp[1] = state[0][c] ^ galois_multiply(state[1][c], 2) ^ galois_multiply(state[2][c], 3) ^ state[3][c];
        tmp[2] = state[0][c] ^ state[1][c] ^ galois_multiply(state[2][c], 2) ^ galois_multiply(state[3][c], 3);
        tmp[3] = galois_multiply(state[0][c], 3) ^ state[1][c] ^ state[2][c] ^ galois_multiply(state[3][c], 2);
        for (int r = 0; r < 4; ++r) state[r][c] = tmp[r];
    }
}

void inv_mix_columns(unsigned char state[4][4]) {
    unsigned char tmp[4];
    for (int c = 0; c < Nb; ++c) {
        tmp[0] = galois_multiply(state[0][c], 14) ^ galois_multiply(state[1][c], 11) ^ galois_multiply(state[2][c], 13) ^ galois_multiply(state[3][c], 9);
        tmp[1] = galois_multiply(state[0][c], 9) ^ galois_multiply(state[1][c], 14) ^ galois_multiply(state[2][c], 11) ^ galois_multiply(state[3][c], 13);
        tmp[2] = galois_multiply(state[0][c], 13) ^ galois_multiply(state[1][c], 9) ^ galois_multiply(state[2][c], 14) ^ galois_multiply(state[3][c], 11);
        tmp[3] = galois_multiply(state[0][c], 11) ^ galois_multiply(state[1][c], 13) ^ galois_multiply(state[2][c], 9) ^ galois_multiply(state[3][c], 14);
        for (int r = 0; r < 4; ++r) state[r][c] = tmp[r];
    }
}

void encrypt_block(const unsigned char* input, unsigned char* output, const unsigned char* round_keys) {
    unsigned char state[4][4];
    BytesToMatrix(input, state);
    add_round_key(state, round_keys, 0);
    for (int round = 1; round < Nr; ++round) {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, round_keys, round);
    }
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, round_keys, Nr);
    MatrixToBytes(state, output);
}

void decrypt_block(const unsigned char* input, unsigned char* output, const unsigned char* round_keys) {
    unsigned char state[4][4];
    BytesToMatrix(input, state);
    add_round_key(state, round_keys, Nr);
    for (int round = Nr - 1; round > 0; --round) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, round_keys, round);
        inv_mix_columns(state);
    }
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, round_keys, 0);
    MatrixToBytes(state, output);
}

void generate_and_save_key(const string& filepath) {
    vector<unsigned char> key(BLOCK_SIZE);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (size_t i = 0; i < BLOCK_SIZE; ++i) key[i] = dis(gen);

    ofstream f(filepath, ios::binary);
    if (!f) {
        cerr << "Ошибка: не удалось создать файл ключа " << filepath << endl;
        exit(1);
    }
    f.write(reinterpret_cast<const char*>(key.data()), key.size());
    f.close();
    cout << "[КЛЮЧ] Сгенерирован Cipher Key (HEX): " << to_hex(key.data(), key.size()) << "\n\n";
}

int main() {
    // Установка кодировки UTF-8 для консоли Windows (опционально)
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif

    string plaintext_str = "Я в своем познании настолько преисполнился, что я как будто бы уже сто триллионов миллиардов лет проживаю на триллионах и триллионах таких же планет, как эта Земля, мне этот мир абсолютно понятен, и я здесь ищу только одного - покоя, умиротворения и вот этой гармонии, от слияния с бесконечно вечным, от созерцания великого фрактального подобия и от вот этого замечательного всеединства существа, бесконечно вечного, куда ни посмотри, хоть вглубь - бесконечно малое, хоть ввысь - бесконечное большое, понимаешь? А ты мне опять со своим вот этим, иди суетись дальше, это твоё распределение, это твой путь и твой горизонт познания и ощущения твоей природы, он несоизмеримо мелок по сравнению с моим, понимаешь? Я как будто бы уже давно глубокий старец, бессмертный, ну или там уже почти бессмертный, который на этой планете от её самого зарождения, ещё когда только Солнце только-только сформировалось как звезда, и вот это газопылевое облако, вот, после взрыва, Солнца, когда оно вспыхнуло, как звезда, начало формировать вот эти коацерваты, планеты, понимаешь, я на этой Земле уже как будто почти пять миллиардов лет живу и знаю её вдоль и поперёк этот весь мир, а ты мне какие-то... мне не важно на твои тачки, на твои яхты, на твои квартиры, там, на твоё благо. Я был на этой планете бесконечным множеством, и круче Цезаря, и круче Гитлера, и круче всех великих, понимаешь, был, а где-то был конченым говном, ещё хуже, чем здесь. Я множество этих состояний чувствую. Где-то я был больше подобен растению, где-то я больше был подобен птице, там, червю, где-то был просто сгусток камня, это всё есть душа, понимаешь? Она имеет грани подобия совершенно многообразные, бесконечное множество. Но тебе этого не понять, поэтому ты езжай себе , мы в этом мире как бы живем разными ощущениями и разными стремлениями, соответственно, разное наше и место, разное и наше распределение. Тебе я желаю все самые крутые тачки чтоб были у тебя, и все самые лучше самки, если мало идей, обращайся ко мне, я тебе на каждую твою идею предложу сотню триллионов, как всё делать. Ну а я всё, я иду как глубокий старец,узревший вечное, прикоснувшийся к Божественному, сам стал богоподобен и устремлен в это бесконечное, и который в умиротворении, покое, гармонии, благодати, в этом сокровенном блаженстве пребывает, вовлеченный во всё и во вся, понимаешь, вот и всё, в этом наша разница. Так что я иду любоваться мирозданием, а ты идёшь преисполняться в ГРАНЯХ каких-то, вот и вся разница, понимаешь, ты не зришь это вечное бесконечное, оно тебе не нужно. Ну зато ты, так сказать, более активен, как вот этот дятел долбящий, или муравей, который очень активен в своей стезе, поэтому давай, наши пути здесь, конечно, имеют грани подобия, потому что всё едино, но я-то тебя прекрасно понимаю, а вот ты меня - вряд ли, потому что я как бы тебя в себе содержу, всю твою природу, она составляет одну маленькую там песчиночку, от того что есть во мне, вот и всё, поэтому давай, ступай, езжай, а я пошел наслаждаться прекрасным осенним закатом на берегу теплой южной реки. Всё, ступай, и я пойду.";
    cout << "Исходный текст: " << plaintext_str << "\n\n";

    generate_and_save_key(KEY_FILE);
    unsigned char cipher_key[16];
    ifstream key_in(KEY_FILE, ios::binary);
    if (!key_in) {
        cerr << "Ошибка: не удалось прочитать файл ключа " << KEY_FILE << endl;
        return 1;
    }
    key_in.read(reinterpret_cast<char*>(cipher_key), 16);
    key_in.close();

    unsigned char round_keys[Nb * 4 * (Nr + 1)];
    expand_key(cipher_key, round_keys);
    for (int r = 0; r <= Nr; ++r) {
        cout << "Ключ для раунда " << r << ": " << to_hex(round_keys + (r * 16), 16) << endl;
    }

    unsigned char iv[BLOCK_SIZE];
    random_device rd;
    for (size_t i = 0; i < BLOCK_SIZE; ++i) iv[i] = rd() % 256;
    cout << "[IV] Вектор инициализации (HEX): " << to_hex(iv, BLOCK_SIZE) << "\n\n";

    vector<unsigned char> data(plaintext_str.begin(), plaintext_str.end());
    size_t pad_len = BLOCK_SIZE - (data.size() % BLOCK_SIZE);
    for (size_t i = 0; i < pad_len; ++i) data.push_back(static_cast<unsigned char>(pad_len));

    cout << "=== ЗАПУСК ШИФРОВАНИЯ (CBC) ===\n";
    vector<unsigned char> ciphertext(data.size());
    unsigned char prev_block[BLOCK_SIZE];
    copy(iv, iv + BLOCK_SIZE, prev_block);

    for (size_t block_offset = 0; block_offset < data.size(); block_offset += BLOCK_SIZE) {
        unsigned char block[BLOCK_SIZE];
        for (size_t i = 0; i < BLOCK_SIZE; ++i)
            block[i] = data[block_offset + i] ^ prev_block[i];

        unsigned char state_matrix[4][4];
        BytesToMatrix(block, state_matrix);
        print_state(state_matrix, "До шифрования, Блок " + to_string(block_offset / BLOCK_SIZE + 1));

        encrypt_block(block, &ciphertext[block_offset], round_keys);
        copy(&ciphertext[block_offset], &ciphertext[block_offset] + BLOCK_SIZE, prev_block);
    }
    cout << "Финальный Ciphertext (HEX): " << to_hex(ciphertext.data(), ciphertext.size()) << "\n\n";

    cout << "=== ЗАПУСК РАСШИФРОВАНИЯ (CBC) ===\n";
    vector<unsigned char> decrypted_padded(ciphertext.size());
    copy(iv, iv + BLOCK_SIZE, prev_block);

    for (size_t block_offset = 0; block_offset < ciphertext.size(); block_offset += BLOCK_SIZE) {
        unsigned char decrypted_block[BLOCK_SIZE];
        decrypt_block(&ciphertext[block_offset], decrypted_block, round_keys);

        for (size_t i = 0; i < BLOCK_SIZE; ++i)
            decrypted_padded[block_offset + i] = decrypted_block[i] ^ prev_block[i];

        unsigned char state_matrix[4][4];
        BytesToMatrix(&decrypted_padded[block_offset], state_matrix);
        print_state(state_matrix, "После расшифровки, Блок " + to_string(block_offset / BLOCK_SIZE + 1));

        copy(&ciphertext[block_offset], &ciphertext[block_offset] + BLOCK_SIZE, prev_block);
    }

    size_t total_pad = decrypted_padded.back();
    if (total_pad > 0 && total_pad <= BLOCK_SIZE)
        decrypted_padded.resize(decrypted_padded.size() - total_pad);

    string result_text(decrypted_padded.begin(), decrypted_padded.end());
    cout << "\nВосстановленный текст: " << result_text << "\n";

    return 0;
}