::  graph-store [landscape]
::
::
/+  store=graph-store, sigs=signatures, res=resource, default-agent, dbug
~%  %graph-store-top  ..is  ~
|%
+$  card  card:agent:gall
+$  versioned-state
  $%  state-0
      state-1
      state-2
  ==
::
+$  state-0  [%0 network:store]
+$  state-1  [%1 network:store]
+$  state-2  [%2 network:store]
::
++  orm      orm:store
++  orm-log  orm-log:store
+$  debug-input  [%validate-graph =resource:store]
--
::
=|  state-2
=*  state  -
::
%-  agent:dbug
^-  agent:gall
~%  %graph-store-agent  ..card  ~
|_  =bowl:gall
+*  this  .
    def   ~(. (default-agent this %|) bowl)
::
++  on-init  [~ this]
++  on-save  !>(state)
++  on-load
  |=  =old=vase
  ^-  (quip card _this)
  =+  !<(old=versioned-state old-vase)
  =|  cards=(list card)
  |^
  ?-    -.old
      %0  
    %_    $
      -.old  %1
    ::
        validators.old
      (~(put in validators.old) %graph-validator-link)
    ::
        cards
      %+  weld  cards
      %+  turn
        ~(tap in (~(put in validators.old) %graph-validator-link))
      |=  validator=@t
      ^-  card
      =/  =wire  /validator/[validator]
      =/  =rave:clay  [%sing %b [%da now.bowl] /[validator]]
      [%pass wire %arvo %c %warp our.bowl [%home `rave]]
    ::
        graphs.old
      %-  ~(run by graphs.old)
      |=  [=graph:store q=(unit mark)]
      ^-  [graph:store (unit mark)]
      :-  (convert-unix-timestamped-graph graph)
      ?^  q  q
      `%graph-validator-link
    ::
        update-logs.old
      %-  ~(run by update-logs.old)
      |=(a=* *update-log:store)
    ==
  ::
      %1
    %_  $
      -.old       %2
      graphs.old  (~(run by graphs.old) change-revision-graph)
    ::
        update-logs.old
      %-  ~(run by update-logs.old)
      |=(a=* *update-log:store)
    ==
  ::
    %2  [cards this(state old)]
  ==
  ::
  ++  change-revision-graph
    |=  [=graph:store q=(unit mark)]
    ^-  [graph:store (unit mark)]
    |^
    :_  q
    ?+    q  graph
      [~ %graph-validator-link]     convert-links
      [~ %graph-validator-publish]  convert-publish
    ==
    ::
    ++  convert-links
      %+  gas:orm  *graph:store
      %+  turn  (tap:orm graph)
      |=  [=atom =node:store]
      ^-  [^atom node:store]
      ::  top-level
      ::
      :+  atom  post.node
      ?:  ?=(%empty -.children.node)
        [%empty ~]
      :-  %graph
      %+  gas:orm  *graph:store
      %+  turn  (tap:orm p.children.node)
      |=  [=^atom =node:store]
      ^-  [^^atom node:store]
      ::  existing comments get turned into containers for revisions
      ::
      :^    atom
          post.node(contents ~, hash ~)
        %graph
      %+  gas:orm  *graph:store
      :_  ~  :-  %1
      :_  [%empty ~]
      post.node(index (snoc index.post.node atom), hash ~)
    ::
    ++  convert-publish
      %+  gas:orm  *graph:store
      %+  turn  (tap:orm graph)
      |=  [=atom =node:store]
      ^-  [^atom node:store]
      ::  top-level
      ::
      :+  atom  post.node
      ?:  ?=(%empty -.children.node)
        [%empty ~]
      :-  %graph
      %+  gas:orm  *graph:store
      %+  turn  (tap:orm p.children.node)
      |=  [=^atom =node:store]
      ^-  [^^atom node:store]
      ::  existing container for publish note revisions
      ::
      ?+    atom  !!
          %1  [atom node]
          %2
        :+  atom  post.node
        ?:  ?=(%empty -.children.node)
          [%empty ~]
        :-  %graph
        %+  gas:orm  *graph:store
        %+  turn  (tap:orm p.children.node)
        |=  [=^^atom =node:store]
        ^-  [^^^atom node:store]
        :+  atom  post.node(contents ~, hash ~)
        :-  %graph
        %+  gas:orm  *graph:store
        :_  ~  :-  %1
        :_  [%empty ~]
        post.node(index (snoc index.post.node atom), hash ~)
      ==
    --
  ::  
  ++  maybe-unix-to-da
    |=  =atom
    ^-  @
    ::  (bex 127) is roughly 226AD
    ?.  (lte atom (bex 127))
      atom
    (add ~1970.1.1 (div (mul ~s1 atom) 1.000))
  ::
  ++  convert-unix-timestamped-node
    |=  =node:store
    ^-  node:store
    =.  index.post.node
      (convert-unix-timestamped-index index.post.node)
    ?.  ?=(%graph -.children.node)
      node
    :+  post.node
      %graph
    (convert-unix-timestamped-graph p.children.node)
  ::
  ++  convert-unix-timestamped-index
    |=  =index:store
    (turn index maybe-unix-to-da)
  ::
  ++  convert-unix-timestamped-graph
    |=  =graph:store
    %+  gas:orm  *graph:store
    %+  turn
      (tap:orm graph)
    |=  [=atom =node:store]
    ^-  [^atom node:store]
    :-  (maybe-unix-to-da atom)
    (convert-unix-timestamped-node node)
  --
::
++  on-watch
  ~/  %graph-store-watch
  |=  =path
  ^-  (quip card _this)
  |^
  ?>  (team:title our.bowl src.bowl)
  =/  cards=(list card)
    ?+  path       (on-watch:def path)
        [%updates ~]   ~
        [%keys ~]      (give [%keys ~(key by graphs)])
        [%tags ~]      (give [%tags ~(key by tag-queries)])
    ==
  [cards this]
  ::
  ++  give
    |=  =update-0:store
    ^-  (list card)
    [%give %fact ~ [%graph-update !>([%0 now.bowl update-0])]]~
  --
::
++  on-poke
  ~/  %graph-store-poke
  |=  [=mark =vase]
  ^-  (quip card _this)
  |^
  ?>  (team:title our.bowl src.bowl)
  =^  cards  state
    ?+  mark           (on-poke:def mark vase)
        %graph-update  (graph-update !<(update:store vase))
        %noun          (debug !<(debug-input vase))
    ==
  [cards this]
  ::
  ++  graph-update
    |=  =update:store
    ^-  (quip card _state)
    |^
    ?>  ?=(%0 -.update)
    =?  p.update  =(p.update *time)  now.bowl
    ?-  -.q.update
        %add-graph          (add-graph +.q.update)
        %remove-graph       (remove-graph +.q.update)
        %add-nodes          (add-nodes p.update +.q.update)
        %remove-nodes       (remove-nodes p.update +.q.update)
        %add-signatures     (add-signatures p.update +.q.update)
        %remove-signatures  (remove-signatures p.update +.q.update)
        %add-tag            (add-tag +.q.update)
        %remove-tag         (remove-tag +.q.update)
        %archive-graph      (archive-graph +.q.update)
        %unarchive-graph    (unarchive-graph +.q.update)
        %run-updates        (run-updates +.q.update)
        %keys               ~|('cannot send %keys as poke' !!)
        %tags               ~|('cannot send %tags as poke' !!)
        %tag-queries        ~|('cannot send %tag-queries as poke' !!)
    ==
    ::
    ++  add-graph
      |=  $:  =resource:store
              =graph:store
              mark=(unit mark:store)
              overwrite=?
          ==
      ^-  (quip card _state)
      ?>  ?|  overwrite
              ?&  !(~(has by archive) resource)
                  !(~(has by graphs) resource)
          ==  ==
      ?>  (validate-graph graph mark)
      :_  %_  state
              graphs       (~(put by graphs) resource [graph mark])
              update-logs  (~(put by update-logs) resource (gas:orm-log ~ ~))
              archive      (~(del by archive) resource)
              validators
            ?~  mark  validators
            (~(put in validators) u.mark)
          ==
      %-  zing
      :~  (give [/updates /keys ~] [%add-graph resource graph mark overwrite])
          ?~  mark  ~
          ?:  (~(has in validators) u.mark)  ~
          =/  wire  /validator/[u.mark]
          =/  =rave:clay  [%sing %b [%da now.bowl] /[u.mark]]
          [%pass wire %arvo %c %warp our.bowl [%home `rave]]~
      ==
    ::
    ++  remove-graph
      |=  =resource:store
      ^-  (quip card _state)
      ?<  (~(has by archive) resource)
      ?>  (~(has by graphs) resource)
      :-  (give [/updates /keys ~] [%remove-graph resource])
      %_  state
          graphs       (~(del by graphs) resource)
          update-logs  (~(del by update-logs) resource)
      ==
    ::
    ++  add-nodes
      |=  $:  =time
              =resource:store
              nodes=(map index:store node:store)
          ==
      ^-  (quip card _state)
      |^
      =/  [=graph:store mark=(unit mark:store)]
        (~(got by graphs) resource)
      =/  =update-log:store  (~(got by update-logs) resource)
      =.  update-log
        (put:orm-log update-log time [%0 time [%add-nodes resource nodes]])
      ::
      :-  (give [/updates]~ [%add-nodes resource nodes])
      %_  state
          update-logs  (~(put by update-logs) resource update-log)
          graphs
        %+  ~(put by graphs)
          resource
        :_  mark
        (add-node-list resource graph mark (sort-nodes nodes))
      ==
      ::
      ++  sort-nodes
        |=  nodes=(map index:store node:store)
        ^-  (list [index:store node:store])
        %+  sort  ~(tap by nodes)
        |=  [p=[=index:store *] q=[=index:store *]]
        ^-  ?
        (lth (lent index.p) (lent index.q))
      ::
      ++  add-node-list
        |=  $:  =resource:store
                =graph:store
                mark=(unit mark:store)
                node-list=(list [index:store node:store])
            ==
        ^-  graph:store
        ?~  node-list  graph
        =*  index  -.i.node-list
        =*  node   +.i.node-list
        %_  $
            node-list  t.node-list
            graph      (add-node-at-index graph index node mark)
        ==
      ::
      ++  add-node-at-index
        =|  parent-hash=(unit hash:store)
        |=  $:  =graph:store
                =index:store
                =node:store
                mark=(unit mark:store)
            ==
        ^-  graph:store
        ?<  ?=(~ index)
        ~|  "validation of node failed using mark {<mark>}"
        ?>  (validate-graph (gas:orm ~ [i.index node]~) mark)
        =*  atom   i.index
        %^  put:orm
            graph
          atom
        ::  add child
        ::
        ?~  t.index
          =*  p  post.node
          =/  =validated-portion:store
            [parent-hash author.p time-sent.p contents.p]
          =/  =hash:store  `@ux`(sham validated-portion)
          ?~  hash.p  node(signatures.post *signatures:store)
          ~|  "signatures do not match the calculated hash"
          ?>  (are-signatures-valid:sigs our.bowl signatures.p hash now.bowl)
          ~|  "hash of post does not match calculated hash"
          ?>  =(hash u.hash.p)
          node
        ::  recurse children
        ::
        =/  parent=node:store
          ~|  "index does not exist to add a node to!"
          (need (get:orm graph atom))
        %_  parent
            children
          ^-  internal-graph:store
          :-  %graph
          %_  $
              index        t.index
              parent-hash  hash.post.parent
              graph
            ?:  ?=(%graph -.children.parent)
              p.children.parent
            (gas:orm ~ ~)
          ==
        ==
      --
    ::
    ++  remove-nodes
      |=  [=time =resource:store indices=(set index:store)]
      ^-  (quip card _state)
      |^
      =/  [=graph:store mark=(unit mark:store)]
        (~(got by graphs) resource)
      =/  =update-log:store  (~(got by update-logs) resource)
      =.  update-log
        (put:orm-log update-log time [%0 time [%remove-nodes resource indices]])
      ::
      :-  (give [/updates]~ [%remove-nodes resource indices])
      %_  state
          update-logs  (~(put by update-logs) resource update-log)
          graphs
        %+  ~(put by graphs)
          resource
        [(remove-indices resource graph ~(tap in indices)) mark]
      ==
      ::
      ++  remove-indices
        |=  [=resource:store =graph:store indices=(list index:store)]
        ^-  graph:store
        ?~  indices  graph
        %_  $
            indices  t.indices
            graph    (remove-index graph i.indices)
        ==
      ::
      ++  remove-index
        |=  [=graph:store =index:store]
        ^-  graph:store
        ?~  index  graph
        =*  atom   i.index
        ::  last index in list
        ::
        ?~  t.index
          +:`[* graph:store]`(del:orm graph atom)
        =/  =node:store
          ~|  "parent index does not exist to remove a node from!"
          (need (get:orm graph atom))
        ~|  "child index does not exist to remove a node from!"
        ?>  ?=(%graph -.children.node)
        %^  put:orm
            graph
          atom
        node(p.children $(graph p.children.node, index t.index))
      --
    ::
    ++  add-signatures
      |=  [=time =uid:store =signatures:store]
      ^-  (quip card _state)
      |^
      =*  resource  resource.uid
      =/  [=graph:store mark=(unit mark:store)]
        (~(got by graphs) resource)
      =/  =update-log:store  (~(got by update-logs) resource)
      =.  update-log
        (put:orm-log update-log time [%0 time [%add-signatures uid signatures]])
      ::
      :-  (give [/updates]~ [%add-signatures uid signatures])
      %_  state
          update-logs  (~(put by update-logs) resource update-log)
          graphs
        %+  ~(put by graphs)  resource
        [(add-at-index graph index.uid signatures) mark]
      ==
      ::
      ++  add-at-index
        |=  [=graph:store =index:store =signatures:store]
        ^-  graph:store
        ?~  index  graph
        =*  atom   i.index
        =/  =node:store
          ~|  "node does not exist to add signatures to!"
          (need (get:orm graph atom))
        ::  last index in list
        ::
        %^  put:orm
            graph
          atom
        ?~  t.index
          ~|  "cannot add signatures to a node missing a hash"
          ?>  ?=(^ hash.post.node)
          ~|  "signatures did not match public keys!"
          ?>  (are-signatures-valid:sigs our.bowl signatures u.hash.post.node now.bowl)
          node(signatures.post (~(uni in signatures) signatures.post.node))
        ~|  "child graph does not exist to add signatures to!"
        ?>  ?=(%graph -.children.node)
        node(p.children $(graph p.children.node, index t.index))
      --
    ::
    ++  remove-signatures
      |=  [=time =uid:store =signatures:store]
      ^-  (quip card _state)
      |^
      =*  resource  resource.uid
      =/  [=graph:store mark=(unit mark:store)]
        (~(got by graphs) resource)
      =/  =update-log:store  (~(got by update-logs) resource)
      =.  update-log
        %^  put:orm-log  update-log
          time
        [%0 time [%remove-signatures uid signatures]]
      ::
      :-  (give [/updates]~ [%remove-signatures uid signatures])
      %_  state
          update-logs  (~(put by update-logs) resource update-log)
          graphs
        %+  ~(put by graphs)  resource
        [(remove-at-index graph index.uid signatures) mark]
      ==
      ::
      ++  remove-at-index
        |=  [=graph:store =index:store =signatures:store]
        ^-  graph:store
        ?~  index  graph
        =*  atom   i.index
        =/  =node:store
          ~|  "node does not exist to add signatures to!"
          (need (get:orm graph atom))
        ::  last index in list
        ::
        %^  put:orm
            graph
          atom
        ?~  t.index
          node(signatures.post (~(dif in signatures) signatures.post.node))
        ~|  "child graph does not exist to add signatures to!"
        ?>  ?=(%graph -.children.node)
        node(p.children $(graph p.children.node, index t.index))
      --
    ::
    ++  add-tag
      |=  [=term =resource:store]
      ^-  (quip card _state)
      ?>  (~(has by graphs) resource)
      :-  (give [/updates /tags ~] [%add-tag term resource])
      %_  state
          tag-queries  (~(put ju tag-queries) term resource)
      ==
    ::
    ++  remove-tag
      |=  [=term =resource:store]
      ^-  (quip card _state)
      ?>  (~(has by graphs) resource)
      :-  (give [/updates /tags ~] [%remove-tag term resource])
      %_  state
          tag-queries  (~(del ju tag-queries) term resource)
      ==
    ::
    ++  archive-graph
      |=  =resource:store
      ^-  (quip card _state)
      ?<  (~(has by archive) resource)
      ?>  (~(has by graphs) resource)
      :-  (give [/updates /keys /tags ~] [%archive-graph resource])
      %_  state
          archive      (~(put by archive) resource (~(got by graphs) resource))
          graphs       (~(del by graphs) resource)
          update-logs  (~(del by update-logs) resource)
          tag-queries
        %-  ~(run by tag-queries)
        |=  =resources:store
        (~(del in resources) resource)
      ==
    ::
    ++  unarchive-graph
      |=  =resource:store
      ^-  (quip card _state)
      ?>  (~(has by archive) resource)
      ?<  (~(has by graphs) resource)
      :-  (give [/updates /keys ~] [%unarchive-graph resource])
      %_  state
          archive      (~(del by archive) resource)
          graphs       (~(put by graphs) resource (~(got by archive) resource))
          update-logs  (~(put by update-logs) resource (gas:orm-log ~ ~))
      ==
    ::
    ++  run-updates
      |=  [=resource:store =update-log:store]
      ^-  (quip card _state)
      ?<  (~(has by archive) resource)
      ?>  (~(has by graphs) resource)
      =/  updates=(list [=time upd=logged-update:store])
        (tap:orm-log update-log)
      =|  cards=(list card)
      |-  ^-  (quip card _state)
      ?~  updates
        [cards state]
      =*  update  upd.i.updates
      =^  crds  state
        %-  graph-update 
        ^-  update:store
        ?-  -.q.update
            %add-nodes          update(resource.q resource)
            %remove-nodes       update(resource.q resource)
            %add-signatures     update(resource.uid.q resource)
            %remove-signatures  update(resource.uid.q resource)
        ==
      $(cards (weld cards crds), updates t.updates)
    ::
    ++  give
      |=  [paths=(list path) update=update-0:store]
      ^-  (list card)
      [%give %fact paths [%graph-update !>([%0 now.bowl update])]]~
    --
  ::
  ++  debug
    |=  =debug-input
    ^-  (quip card _state)
    =/  [=graph:store mark=(unit mark:store)]
      (~(got by graphs) resource.debug-input)
    ?>  (validate-graph graph mark)
    [~ state]
  ::
  ++  validate-graph
    |=  [=graph:store mark=(unit mark:store)]
    ^-  ?
    ?~  mark   %.y
    ?~  graph  %.y
    =/  =dais:clay
      .^  =dais:clay
          %cb
          /(scot %p our.bowl)/[q.byk.bowl]/(scot %da now.bowl)/[u.mark]
      ==
    %+  roll  (tap:orm graph)
    |=  [[=atom =node:store] out=?]
    ?&  out
        =(%& -:(mule |.((vale:dais [atom post.node]))))
        ?-  -.children.node
            %empty  %.y
            %graph  ^$(graph p.children.node)
        ==
    ==
  --
::
++  on-peek
  ~/  %graph-store-peek
  |=  =path
  ^-  (unit (unit cage))
  |^
  ?>  (team:title our.bowl src.bowl)
  ?+  path  (on-peek:def path)
      [%x %graph-mark @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  result=(unit marked-graph:store)
      (~(get by graphs) [ship term])
    ?~  result  [~ ~]
    ``noun+!>(q.u.result)
  ::
      [%x %keys ~]
    :-  ~  :-  ~  :-  %graph-update
    !>(`update:store`[%0 now.bowl [%keys ~(key by graphs)]])
  ::
      [%x %tags ~]
    :-  ~  :-  ~  :-  %graph-update
    !>(`update:store`[%0 now.bowl [%tags ~(key by tag-queries)]])
  ::
      [%x %tag-queries ~]
    :-  ~  :-  ~  :-  %graph-update
    !>(`update:store`[%0 now.bowl [%tag-queries tag-queries]])
  ::
      [%x %graph @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  result=(unit marked-graph:store)
      (~(get by graphs) [ship term])
    ?~  result  [~ ~]
    :-  ~  :-  ~  :-  %graph-update
    !>  ^-  update:store
    :+  %0
      now.bowl
    [%add-graph [ship term] `graph:store`p.u.result q.u.result %.y]
  ::
      ::  note: near-duplicate of /x/graph
      ::
      [%x %archive @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  result=(unit marked-graph:store)
      (~(get by archive) [ship term])
    ?~  result
      ~&  no-archived-graph+[ship term]
      [~ ~]
    :-  ~  :-  ~  :-  %graph-update
    !>  ^-  update:store
    :+  %0
      now.bowl
    [%add-graph [ship term] `graph:store`p.u.result q.u.result %.y]
  ::
      [%x %graph-subset @ @ @ @ ~]
    =/  =ship  (slav %p i.t.t.path)
    =/  =term  i.t.t.t.path
    =/  start=(unit atom)  (rush i.t.t.t.t.path dem:ag)
    =/  end=(unit atom)    (rush i.t.t.t.t.t.path dem:ag)
    =/  graph=(unit marked-graph:store)
      (~(get by graphs) [ship term])
    ?~  graph  [~ ~]
    :-  ~  :-  ~  :-  %graph-update
    !>  ^-  update:store
    :+  %0  now.bowl
    :+  %add-nodes
      [ship term]
    %-  ~(gas by *(map index:store node:store))
    %+  turn  (tap:orm `graph:store`(subset:orm p.u.graph start end))
    |=  [=atom =node:store]
    ^-  [index:store node:store]
    [~[atom] node]
  ::
      [%x %node @ @ @ *]
    =/  =ship  (slav %p i.t.t.path)
    =/  =term  i.t.t.t.path
    =/  =index:store
      (turn t.t.t.t.path (cury slav %ud))
    =/  node=(unit node:store)  (get-node ship term index)
    ?~  node  [~ ~]
    :-  ~  :-  ~  :-  %graph-update
    !>  ^-  update:store
    :+  %0
      now.bowl
    :+  %add-nodes
      [ship term]
    (~(gas by *(map index:store node:store)) [index u.node] ~)
  ::
      [%x %node-children-subset @ @ @ @ @ *]
    =/  =ship  (slav %p i.t.t.path)
    =/  =term  i.t.t.t.path
    =/  start=(unit atom)  (rush i.t.t.t.t.path dem:ag)
    =/  end=(unit atom)    (rush i.t.t.t.t.t.path dem:ag)
    =/  =index:store
      (turn t.t.t.t.t.t.path |=(=cord (slav %ud cord)))
    =/  node=(unit node:store)  (get-node ship term index)
    ?~  node  [~ ~]
    ?-  -.children.u.node
        %empty  [~ ~]
        %graph
      :-  ~  :-  ~  :-  %graph-update
      !>  ^-  update:store
      :+  %0
        now.bowl
      :+  %add-nodes
        [ship term]
      %-  ~(gas by *(map index:store node:store))
      %+  turn  (tap:orm `graph:store`(subset:orm p.children.u.node end start))
      |=  [=atom =node:store]
      ^-  [index:store node:store]
      [(snoc index atom) node]
    ==
  ::
      [%x %update-log-subset @ @ @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  start=(unit time)  (slaw %da i.t.t.t.t.path)
    =/  end=(unit time)    (slaw %da i.t.t.t.t.t.path)
    =/  update-log=(unit update-log:store)  (~(get by update-logs) [ship term])
    ?~  update-log  [~ ~]
    ::  orm-log is ordered backwards, so swap start and end
    ``noun+!>((subset:orm-log u.update-log end start))
  ::
      [%x %update-log @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  update-log=(unit update-log:store)  (~(get by update-logs) [ship term])
    ?~  update-log  [~ ~]
    ``noun+!>(u.update-log)
  ::
      [%x %peek-update-log @ @ ~]
    =/  =ship   (slav %p i.t.t.path)
    =/  =term   i.t.t.t.path
    =/  update-log=(unit update-log:store)  (~(get by update-logs) [ship term])
    ?~  update-log  [~ ~]
    =/  result=(unit [time update:store])
      (peek:orm-log:store u.update-log)
    ?~  result  [~ ~]
    ``noun+!>([~ -.u.result])
  ==
  ::
  ++  get-node
    |=  [=ship =term =index:store]
    ^-  (unit node:store)
    =/  parent-graph=(unit marked-graph:store)
      (~(get by graphs) [ship term])
    ?~  parent-graph  ~
    =/  node=(unit node:store)  ~
    =/  =graph:store  p.u.parent-graph
    |-
    ?~  index
      node
    ?~  t.index
      (get:orm graph i.index)
    =.  node  (get:orm graph i.index)
    ?~  node  ~
    ?-  -.children.u.node
        %empty  ~
        %graph  $(graph p.children.u.node, index t.index)
    ==
  --
::
++  on-arvo
  |=  [=wire =sign-arvo]
  ^-  (quip card _this)
  ?+  wire  (on-arvo:def wire sign-arvo)
  ::
  ::  old wire, do nothing 
      [%graph *]  [~ this]
  ::
      [%validator @ ~]
    :_  this
    =*  validator  i.t.wire
    =/  =rave:clay  [%next %b [%da now.bowl] /[validator]]
    [%pass wire %arvo %c %warp our.bowl [%home `rave]]~
  ==
::
++  on-agent  on-agent:def
++  on-leave  on-leave:def
++  on-fail   on-fail:def
--
