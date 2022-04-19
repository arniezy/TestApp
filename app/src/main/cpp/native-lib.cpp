#include <jni.h>
#include <string>
#include <stdio.h>
#include "tinyxml2.h"
using namespace tinyxml2;

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_test_1app_MainActivity_createPhoneBook(JNIEnv *env, jobject) {
    jclass phoneClass = env->FindClass("com/example/test_app/PhonebookEntry");

    // генерируем число записей в диапазоне от 20 до 30
    srand(time(nullptr));
    int num_entries = rand() % 11 + 20;

    // создаем нужный массив
    jobjectArray phoneArray = env->NewObjectArray(num_entries, phoneClass, nullptr);

    for (int i = 0; i < num_entries; i++) {
        std::string cur_surname, cur_number;

        // генерируем фамилию - случайный набор букв от 4 до 12 символов
        cur_surname += static_cast<char>(rand() % 26 + 'A'); // 1-я буква фамилии
        int name_length = rand() % 9 + 4;       // rand() % (max - min + 1) + min

        for (int j = 0; j < name_length; j++)
            cur_surname +=  static_cast<char>(rand() % 26 + 'a');

        // генерируем номер телефона - случайный набор цифр
        for (int i = 0; i < 11; i++)
            cur_number += rand() % 10 + '0';

        env->SetObjectArrayElement(phoneArray, i, env->NewObject(phoneClass,
                                                              env->GetMethodID(phoneClass, "<init>",
                                                                               "(Ljava/lang/String;Ljava/lang/String;)V"),
                                                              env->NewStringUTF(cur_surname.c_str()),
                                                              env->NewStringUTF(cur_number.c_str())));
    }

    return phoneArray;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_test_1app_MainActivity_searchInPhoneBook(JNIEnv *env, jobject thiz, jstring str,
                                                          jobjectArray arr) {
    //создаем контейнер новый
    jclass phoneClass = env->FindClass("com/example/test_app/PhonebookEntry");
    jmethodID method = env->GetMethodID(phoneClass, "Compare", "(Ljava/lang/String;)Z");

    jmethodID methodName = env->GetMethodID(phoneClass, "GetSurname", "()Ljava/lang/String;");
    jmethodID methodNumber = env->GetMethodID(phoneClass, "GetPhone", "()Ljava/lang/String;");

    XMLDocument doc;

    const char* xmlPath = "src/main/res/values/tmpRes.xml";
    doc.LoadFile(xmlPath);
    doc.Clear();

    XMLDeclaration* declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration);

    XMLElement* root = doc.NewElement("data");
    doc.InsertEndChild(root);

    //начинаем просматривать весь массив
    //если подходит - добавляем
    int size = env->GetArrayLength(arr);
    for (int i = 0; i < size; i++) {
        jobject cur_obj = env->GetObjectArrayElement(arr, i);

        if (env->CallBooleanMethod(cur_obj, method, str)) {

            XMLElement* contactNode = doc.NewElement("contact");
            contactNode->SetAttribute("name", reinterpret_cast<const char *>(env->CallObjectMethod(cur_obj, methodName)));
            contactNode->SetAttribute("number",  reinterpret_cast<const char *>(env->CallObjectMethod(cur_obj, methodNumber)));
            root->InsertEndChild(contactNode);
        }
    }

    //возвращение файла с результатом
    doc.SaveFile(xmlPath);
    return (jstring)xmlPath;
}