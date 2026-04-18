1. javac *.java
2. rmiregistry (run in separate terminal)
3. java -Djava.security.policy=security.policy RMIServer
4. java -Djava.security.policy=security.policy RMIClient