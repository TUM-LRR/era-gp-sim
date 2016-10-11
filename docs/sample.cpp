
/**
*
* Kurze Beschreibung (1-2 kurze Saetze), was die Klasse macht.
*
* Falls es hier noch was zu erlaeutern gibt, dann habt ihr hier
* Platz. Z.B. koennten hier wichtige Implementierungsdetails- oder
* Entscheidungen rein, von denen jemand, der die Klasse zum ersten Mal sieht,
* wissen sollte. Oder einfach alles, was ihr euch selber wissen lassen moechtet,
* wenn ihr die Klasse in 30 Jahren aus dem Staub ausgrabt und
* weiterprogrammieren muesst.
*
* "We decided to use a
* lock-free-randomized-double-hashed-binary-graph-heap-queue-tree because it
* seemed to influence the quality of the lunch we had in the cafeteria. My
* analysis showed that insertion runs in O(very fast) while deletion asymptotes
* at around O(slow)."
*
* __Markdown__ wird uebrigens *supported*!
*
*
*/
class MyAmazingClass {
 public:
  /**
   * Kurze Beschreibung.
   *
   * Hier wieder mehr Information. Doxygen hat natuerlich wie Javadoc noch viele
   * andere Befehle. Minimum sollten die @param sein und und wenn die Funktion
   * nicht `void` returned auch der @return Tag. Eine volle Liste findet ihr
   * [hier](https://www.stack.nl/~dimitri/doxygen/manual/commands.html).
   *
   *
   * @tparam TemplateParameter Beschreibung eines Template Parameters.
   *
   * @param parameter1 Beschreibung des ersten Parameters.
   * @param parameter2 Beschreibung des zweiten Parameters.
   *
   * @return Beschreibung des Rueckgabewerts.
   *
   * @throws ExceptionType Wenn ihr wollt, hier eine Beschreibung von potentiell
   *         geworfenen Exceptions.
   *
   * @see Hier koennt ihr eine Funktion listen, die man sich am besten auch noch
   *      ansehen sollte.
   * @see bar()
   */
  template <typename TemplateParameter>
  ReturnType foo(Parameter1 parameter1, Parameter2 parameter2);

  /** Inline Doxygen zur Beschreibung eines Members. */
  std::vector<Type> _vector;
};
