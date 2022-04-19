package com.example.test_app

import android.content.Intent
import android.os.Bundle
import android.util.Xml
import android.widget.ListView
import androidx.appcompat.app.AppCompatActivity
import com.example.test_app.databinding.ActivityMainBinding
import org.xmlpull.v1.XmlPullParser
import org.xmlpull.v1.XmlPullParserException


class PhonebookEntry(val name: String, val number: String) {
    public fun Compare(str: String): Boolean {
        val nextStr = str.dropLast(1) + (str.codePointAt(str.length - 1) + 1).toChar();
        if (name >= str && name < nextStr)
            return true
        return false
    }

    public fun GetSurname(): String { return name }

    public fun GetPhone(): String { return number }
}

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    // функция генерации случайной телефонной книги
    external fun createPhoneBook(): Array<PhonebookEntry>

    // функция поиска необходимых данных
    // str - строка для поиска,
    // возвращает имя файла, где содержатся нужные данные
    external fun searchInPhoneBook(str: String, arr: Array<PhonebookEntry>): String

    private fun createXmlPullParser(fileName: String): XmlPullParser {
        val parser = Xml.newPullParser()

        // получаем доступ к xml файлу
        val inputStream = applicationContext.assets.open(fileName)
        parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false)
        parser.setInput(inputStream, null)
        return parser
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.buttonToInfo.setOnClickListener {
            val intent = Intent(this, InfoActivity::class.java)
            startActivity(intent)
        }

        binding.buttonSearch.setOnClickListener {
            //читаем данные из строки
            val searchStr = binding.editTextTextPersonName.getText().toString()

            //удаляем пробелы в начале и конце строки
            searchStr.removePrefix(" ");
            searchStr.removeSuffix(" ");

            //если строка не пустая, то выполняем поиск
            if (searchStr != "") {
                val listView: ListView = findViewById(R.id.listView)

                val fileName: String = searchInPhoneBook(searchStr, Phonebook)
                val parser = createXmlPullParser(fileName)

                var arrayList: ArrayList<String> = arrayListOf<String>()

                while (parser.eventType != XmlPullParser.END_DOCUMENT) {
                    if (parser.eventType == XmlPullParser.START_TAG && parser.name == "contact") {
                        arrayList.add(
                            parser.getAttributeValue(0)
                                .toString() + "\n" + parser.getAttributeValue(1).toString()
                        )
                    }
                    parser.next()
                }
            }
        }

        // генерируем случайный список номеров телефонов
        Phonebook = createPhoneBook();
    }

    companion object {
        lateinit var Phonebook: Array<PhonebookEntry> // статическая переменная для номеров телефона

        init {
            System.loadLibrary("test_app")
        }
    }
}