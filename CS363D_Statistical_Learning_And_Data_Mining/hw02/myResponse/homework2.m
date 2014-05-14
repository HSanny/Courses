function y = homework2 ()

%% data input
c1 = {'released' 'software' 'computer'};
c2 = {'operating' 'system' 'computer' 'software'};
c3 = {'computer','virus', 'business'};
c4 = {'system','released', 'operating', 'system'};
c5 = {'released', 'virus', 'system'};
m6 = {'recovery', 'economy'};
m7 = {'economy', 'recession'};
m8 = {'growth', 'economy', 'recession'};
m9 = {'economy', 'growth'};
m10 = {'growth', 'business'};

documents = {c1, c2, c3, c4, c5, m6, m7, m8, m9, m10};

%% keyword generation
collections = [c1, c2, c3, c4, c5, m6, m7, m8, m9, m10];
keywords = unique(collections);
keywords

%% 1(a). generate A matrix
A = [];
for i = 1:size(documents,2),
    term_vector = zeros(size(keywords));
    document = documents{i};
    for j = 1:size(document,2),
        word = document{j};
        indx = find(strcmp(word, keywords));
        term_vector(indx) = term_vector(indx) + 1;
    end
    A = [A term_vector'];
end
A

%% 1(b). Normalize matrix A to get matrix B,
%  where each column (document) vector of B has unit l -norm.
%for col = 1:size(A,2),
%    ltnorm = norm(A(:,col));
%    B(:,col) = A(:,col) / ltnorm;
%end
B = normc(A)
    

%% 2. Cosine Similarity
cosineSimilarity = B'* B

%% 3. Singular Value Decomposition
[U,Sigma,V] = svd(B);
disp('Left Singular Vector:')
U

disp('Right Singular Vector:')
V

disp('Singluar Values: ')
singularValues = [];
for i = 1:min(size(Sigma)),
    singularValues = [singularValues Sigma(i,i)];
end
disp(singularValues)

%% 4. Plot the first two left and right singular vectors
U1 = U(:,1)'
U2 = U(:,2)'
a=subplot(1,2,1)
title(a, 'Two Left Singular Vectors')
plot(1:size(U1,2), U1, '-', 1:size(U2,2), U2, '--')
xlabel('dimensional index')
ylabel('dimensional value')
legend('First Left Singular Vector', 'Second Left Singular Vector', 4)
hold on

V1 = V(:,1)'
V2 = V(:,2)'
b=subplot(1,2,2)
title(b,'Two Right Singular Vectors')
plot(1:size(V1,2), V1, '-', 1:size(V2,2), V2, '--')
xlabel('dimensional index')
ylabel('dimensional value')
legend('First Right Singular Vector', 'Second Right Singular Vector', 4)
hold on

%% 5. Plot the projected document vectors in the space 
% spanned by the first two left singular vectors.
docNames = {'c1', 'c2', 'c3', 'c4', 'c5', 'm6', 'm7', 'm8', 'm9', 'm10'};
figure;
for i = 1:size(B,2),
   doc_vector = B(:,i);
   x = U1 * doc_vector;
   y = U2 * doc_vector;
   plot([x],[y],'x')
   text(x+0.005,y, docNames{i})
   hold on
end
xlabel('First Left Singular Vector')
ylabel('Second Left Singular Vector')


%% 6. Plot the projected term vectors in the space 
% spanned by the first two right singular vectors.
figure;
hold on
for i = 1:size(B,1),
   term_vector = B(i,:);
   x = term_vector * V1';
   y = term_vector * V2';
   plot([x],[y],'x')
   text(x+0.005,y, keywords(i))
   hold on
end
xlabel('First Right Singular Vector')
ylabel('Second Right Singular Vector')

