(let [log-path "G:\\projects\\log.txt"
      res-path "G:\\projects\\log-res.txt"
      dict {"=========================="                :it-div
            "--------------------------"                :splitter
            "---------NEW VALS---------"                :new-vals
            "---------TOLERANCE LIMIT REACHED---------" [:fin true]}
      parse (fn [s]
              (when-let [found (re-find #"^([^\:\=\-\d]+)\s*[\:\=]\s*([-\d\.\se]+)$" s)]
                [(->
                   found
                   second
                   clojure.string/trim
                   clojure.string/lower-case
                   (clojure.string/replace #"[_\s]" "-")
                   keyword)
                 (let [v (mapv #(Double/parseDouble %)
                               (clojure.string/split (last found) #"\s"))]
                   (if (seq (rest v)) v (first v)))]))
      parser (fn [s]
               (if-let [v (dict s)]
                 v (parse s)))
      extract-iteration
      (fn [info]
        (let [info (partition-by #{:new-vals} info)
              old (->> info first (remove keyword?) (into {}))
              new (->> info last (remove keyword?) (into {}))]
          {:old old :new new}))
      iterations (->>
                   (slurp log-path)
                   (clojure.string/split-lines)
                   (map parser)
                   (remove #{:splitter})
                   (keep identity)
                   (partition-by #{:it-div})
                   (filter #(< 2 (count %)))
                   (mapv extract-iteration))]
  (->> iterations
       (map (fn [i]
              [(-> i :new :best-total-error)
               (-> i :new :best-max-error)
               (-> i :new :sum-diff)]))
       (map #(apply str (interleave % (repeat " "))))
       (clojure.string/join "\n")
       (spit res-path)
       ))