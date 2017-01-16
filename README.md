# The Compact Message Format bindings project

Bitcoin Classic introduced the Compact Message Format as a very simple
but powerful format to encode and decode any type of messages.

The compact message format is a key/value-pair based format. Each key/value
pair is called a token and a message is build up of a series of tokens.

Take this as a short example, a message with 3 tokens. Each having a name
and a value.
<pre>
   Name=Paris
   Population=2229621
   Area=105.6
</pre>

In all object oriented languages there are very similar constructions
available to create or parse the messages. Please refer to the API docs of
your language bindings for details. I'll give a generic example here, to
explain the concepts.

# Message Creation

For creation of messages we use the [builder
pattern](https://en.wikipedia.org/wiki/Builder_pattern) in the form of the
`MessageBuilder` class.

The MessageBuilder class has a series of `add()` methods each of which
appends a token to your message.

<pre>
  byte[] bytes = new byte[100];
  MessageBuilder builder = new MessageBuilder(bytes);
  builder.add(City.Name, "Paris");
  builder.add(City.Population, 2229621);
  builder.add(City.Area, 105.6);
  builder.close();
</pre>

This allows really easy to read and understand code.

# Message Parsing

The MessageParser is using more of a SOX parser approach where you call
`MessageParser.Next()` and then you can ask the parser for the tag-is and
the actual value.

<pre>
  MessageParser parser = new MessageParser(inputMessage);
  while (parser.next() == MessageParser.FoundTag) {
     if (parser.tag() == City.Population) {
         int population = parser.data.toInt();
         break;
     }
  }
</pre>


At this time there are implementations for;

* C++ which depend on Qt
* C++ with boost
* C# Should work with any version, the project assumes 4.5
* Java
* Python
* C
